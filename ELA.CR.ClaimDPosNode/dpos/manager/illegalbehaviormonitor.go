package manager

import (
	"bytes"

	"github.com/elastos/Elastos.ELA/blockchain"
	"github.com/elastos/Elastos.ELA/common"
	"github.com/elastos/Elastos.ELA/core/contract/program"
	"github.com/elastos/Elastos.ELA/core/types"
	dmsg "github.com/elastos/Elastos.ELA/dpos/p2p/msg"
	"github.com/elastos/Elastos.ELA/errors"
	"github.com/elastos/Elastos.ELA/p2p/msg"
)

const WaitHeightTolerance = uint32(1)

type IllegalBehaviorMonitor interface {
	IsBlockValid(block *types.Block) bool

	AddProposal(proposal types.DPosProposal)
	Reset(changeView bool)

	IsLegalProposal(p *types.DPosProposal) (*types.DPosProposal, bool)
	ProcessIllegalProposal(first, second *types.DPosProposal)

	ProcessIllegalVote(first, second *types.DPosProposalVote)
	IsLegalVote(v *types.DPosProposalVote) (*types.DPosProposalVote, bool)
	SendSidechainIllegalEvidenceTransaction(evidence *types.SidechainIllegalData)

	AddEvidence(evidence types.DposIllegalData)
	SetInactiveArbitratorsTxHash(hash common.Uint256)
}

type illegalBehaviorMonitor struct {
	dispatcher      *proposalDispatcher
	cachedProposals map[common.Uint256]*types.DPosProposal

	evidenceCache evidenceCache
	manager       DposManager

	inactiveArbitratorsTxHash *common.Uint256
}

func (i *illegalBehaviorMonitor) AddEvidence(evidence types.DposIllegalData) {
	i.evidenceCache.AddEvidence(evidence)
}

func (i *illegalBehaviorMonitor) SetInactiveArbitratorsTxHash(
	hash common.Uint256) {
	i.inactiveArbitratorsTxHash = &hash
}

func (i *illegalBehaviorMonitor) IsBlockValid(block *types.Block) bool {
	if i.inactiveArbitratorsTxHash != nil {
		hasInactiveArbitratorsTx := false
		for _, tx := range block.Transactions {
			if tx.Hash().IsEqual(*i.inactiveArbitratorsTxHash) {
				hasInactiveArbitratorsTx = true
			}
		}

		if !hasInactiveArbitratorsTx {
			return false
		}
	}

	return i.evidenceCache.IsBlockValid(block)
}

func (i *illegalBehaviorMonitor) AddProposal(proposal types.DPosProposal) {
	i.cachedProposals[proposal.Hash()] = &proposal
}

func (i *illegalBehaviorMonitor) Reset(changeView bool) {
	i.cachedProposals = make(map[common.Uint256]*types.DPosProposal)
	for k, v := range i.dispatcher.pendingProposals {
		i.cachedProposals[k] = &v
	}

	if !changeView {
		if i.dispatcher.processingBlock != nil {
			i.evidenceCache.Reset(i.dispatcher.processingBlock)
		}

		i.inactiveArbitratorsTxHash = nil
	}
}

func (i *illegalBehaviorMonitor) IsLegalProposal(p *types.DPosProposal) (*types.DPosProposal, bool) {
	if i.isProposalsIllegal(p, i.dispatcher.processingProposal) {
		return i.dispatcher.processingProposal, false
	}

	for _, pending := range i.dispatcher.pendingProposals {
		if i.isProposalsIllegal(p, &pending) {
			return &pending, false
		}
	}

	return nil, true
}

func (i *illegalBehaviorMonitor) ProcessIllegalProposal(first, second *types.DPosProposal) {
	firstBlock, _ := i.dispatcher.cfg.Manager.GetBlockCache().TryGetValue(first.BlockHash)
	secondBlock, _ := i.dispatcher.cfg.Manager.GetBlockCache().TryGetValue(second.BlockHash)

	evidences := &types.DposIllegalProposals{
		Evidence: types.ProposalEvidence{
			Proposal:    *first,
			BlockHeader: firstBlock.Header,
		},
		CompareEvidence: types.ProposalEvidence{
			Proposal:    *second,
			BlockHeader: secondBlock.Header,
		},
	}

	i.AddEvidence(evidences)
	i.sendIllegalProposalTransaction(evidences)

	m := &dmsg.IllegalProposals{Proposals: *evidences}
	i.dispatcher.cfg.Network.BroadcastMessage(m)
}

func (i *illegalBehaviorMonitor) sendIllegalProposalTransaction(evidences *types.DposIllegalProposals) {
	tx := &types.Transaction{
		Version:        types.TransactionVersion(blockchain.DefaultLedger.HeightVersions.GetDefaultTxVersion(i.dispatcher.processingBlock.Height)),
		TxType:         types.IllegalProposalEvidence,
		PayloadVersion: types.PayloadIllegalProposalVersion,
		Payload:        &types.PayloadIllegalProposal{DposIllegalProposals: *evidences},
		Attributes:     []*types.Attribute{},
		LockTime:       0,
		Programs:       []*program.Program{},
		Outputs:        []*types.Output{},
		Inputs:         []*types.Input{},
		Fee:            0,
	}

	if code := i.manager.AppendToTxnPool(tx); code == errors.Success {
		i.manager.Broadcast(msg.NewTx(tx))
	}
}

func (i *illegalBehaviorMonitor) SendSidechainIllegalEvidenceTransaction(evidence *types.SidechainIllegalData) {
	tx := &types.Transaction{
		Version:        types.TransactionVersion(blockchain.DefaultLedger.HeightVersions.GetDefaultTxVersion(i.dispatcher.processingBlock.Height)),
		TxType:         types.IllegalSidechainEvidence,
		PayloadVersion: types.PayloadSidechainIllegalDataVersion,
		Payload:        &types.PayloadSidechainIllegalData{SidechainIllegalData: *evidence},
		Attributes:     []*types.Attribute{},
		LockTime:       0,
		Programs:       []*program.Program{},
		Outputs:        []*types.Output{},
		Inputs:         []*types.Input{},
		Fee:            0,
	}

	if code := i.manager.AppendToTxnPool(tx); code == errors.Success {
		i.manager.Broadcast(msg.NewTx(tx))
	}
}

func (i *illegalBehaviorMonitor) sendIllegalVoteTransaction(evidences *types.DposIllegalVotes) {
	tx := &types.Transaction{
		Version:        types.TransactionVersion(blockchain.DefaultLedger.HeightVersions.GetDefaultTxVersion(i.dispatcher.processingBlock.Height)),
		TxType:         types.IllegalVoteEvidence,
		PayloadVersion: types.PayloadIllegalVoteVersion,
		Payload:        &types.PayloadIllegalVote{DposIllegalVotes: *evidences},
		Attributes:     []*types.Attribute{},
		LockTime:       0,
		Programs:       []*program.Program{},
		Outputs:        []*types.Output{},
		Inputs:         []*types.Input{},
		Fee:            0,
	}

	if code := i.manager.AppendToTxnPool(tx); code == errors.Success {
		i.manager.Broadcast(msg.NewTx(tx))
	}
}

func (i *illegalBehaviorMonitor) ProcessIllegalVote(first, second *types.DPosProposalVote) {
	firstProposal, _ := i.cachedProposals[first.ProposalHash]
	secondProposal, _ := i.cachedProposals[second.ProposalHash]
	firstBlock, _ := i.dispatcher.cfg.Manager.GetBlockCache().TryGetValue(firstProposal.BlockHash)
	secondBlock, _ := i.dispatcher.cfg.Manager.GetBlockCache().TryGetValue(secondProposal.BlockHash)

	evidences := &types.DposIllegalVotes{
		Evidence: types.VoteEvidence{
			Vote:        *first,
			Proposal:    *firstProposal,
			BlockHeader: firstBlock.Header,
		},
		CompareEvidence: types.VoteEvidence{
			Vote:        *second,
			Proposal:    *secondProposal,
			BlockHeader: secondBlock.Header,
		},
	}

	i.AddEvidence(evidences)
	i.sendIllegalVoteTransaction(evidences)

	m := &dmsg.IllegalVotes{Votes: *evidences}
	i.dispatcher.cfg.Network.BroadcastMessage(m)
}

func (i *illegalBehaviorMonitor) isProposalsIllegal(first, second *types.DPosProposal) bool {
	if first == nil || second == nil {
		return false
	}

	if !bytes.Equal(first.Sponsor, second.Sponsor) || first.ViewOffset != second.ViewOffset {
		return false
	}

	firstBlock, foundFirst := i.dispatcher.cfg.Manager.GetBlockCache().TryGetValue(first.BlockHash)
	secondBlock, foundSecond := i.dispatcher.cfg.Manager.GetBlockCache().TryGetValue(second.BlockHash)
	if !foundFirst || !foundSecond {
		return false
	}

	if firstBlock.Height == secondBlock.Height {
		return true
	}

	return false
}

func (i *illegalBehaviorMonitor) IsLegalVote(v *types.DPosProposalVote) (*types.DPosProposalVote, bool) {
	for _, accept := range i.dispatcher.acceptVotes {
		if i.isVotesIllegal(v, &accept) {
			return &accept, false
		}
	}

	for _, reject := range i.dispatcher.rejectedVotes {
		if i.isVotesIllegal(v, &reject) {
			return &reject, false
		}
	}

	return nil, true
}

func (i *illegalBehaviorMonitor) isVotesIllegal(first, second *types.DPosProposalVote) bool {
	if !bytes.Equal(first.Signer, second.Signer) {
		return false
	}

	if first.ProposalHash.IsEqual(second.ProposalHash) {
		return !first.Hash().IsEqual(second.Hash())
	}

	firstProposal, foundFirst := i.cachedProposals[first.ProposalHash]
	secondProposal, foundSecond := i.cachedProposals[second.ProposalHash]
	if !foundFirst || !foundSecond {
		return false
	}

	return i.isProposalsIllegal(firstProposal, secondProposal)
}
