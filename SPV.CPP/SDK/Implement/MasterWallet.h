// Copyright (c) 2012-2018 The Elastos Open Source Project
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef __ELASTOS_SDK_MASTERWALLET_H__
#define __ELASTOS_SDK_MASTERWALLET_H__

#include <map>
#include <boost/shared_ptr.hpp>
#include <boost/filesystem.hpp>
#include <SDK/Wrapper/Transaction.h>

#include "MasterPubKey.h"
#include "Interface/IMasterWallet.h"
#include "Interface/IIdAgent.h"
#include "SDK/KeyStore/Mnemonic.h"
#include "KeyStore/KeyStore.h"
#include "KeyStore/MasterWalletStore.h"
#include "IdAgent/IdAgentImpl.h"

namespace Elastos {
	namespace SDK {

		class CoinInfo;
		class ChainParams;
		class SubWallet;
		class KeyStore;

		class MasterWallet : public IMasterWallet, public IIdAgent {
		public:
			virtual ~MasterWallet();

			bool Initialized() const;

			void Save();

		public: //override from IMasterWallet

			virtual std::string GetId() const;

			virtual std::vector<ISubWallet *> GetAllSubWallets() const;

			virtual ISubWallet *CreateSubWallet(
					const std::string &chainID,
					const std::string &payPassword,
					bool singleAddress,
					uint64_t feePerKb = 0);

			virtual ISubWallet *RecoverSubWallet(
					const std::string &chainID,
					const std::string &payPassword,
					bool singleAddress,
					uint32_t limitGap,
					uint64_t feePerKb = 0);

			virtual void DestroyWallet(ISubWallet *wallet);

			virtual std::string GetPublicKey();

			virtual std::string Sign(
					const std::string &message,
					const std::string &payPassword);

			virtual nlohmann::json CheckSign(
					const std::string &publicKey,
					const std::string &message,
					const std::string &signature);

			virtual bool IsAddressValid(const std::string &address);

		public: //override from IIdAgent
			virtual std::string DeriveIdAndKeyForPurpose(
					uint32_t purpose,
					uint32_t index,
					const std::string &payPassword);

			virtual bool IsIdValid(const std::string &id);

			virtual nlohmann::json GenerateProgram(
					const std::string &id,
					const std::string &message,
					const std::string &password);

			virtual std::string Sign(
					const std::string &id,
					const std::string &message,
					const std::string &password);

			virtual std::vector<std::string> GetAllIds() const;

		protected:

			friend class MasterWalletManager;

			friend class WalletFactoryInner;

			friend class IdAgentImpl;

			friend class SubWallet;

			typedef std::map<std::string, ISubWallet *> WalletMap;

			MasterWallet(const boost::filesystem::path &localStore);

			MasterWallet(const std::string &id,
						 const std::string &language);

			MasterWallet(const std::string &id,
						 const std::string &phrasePassword,
						 const std::string &payPassword,
						 const std::string &language);

			bool importFromKeyStore(const std::string &keystorePath,
									const std::string &backupPassword,
									const std::string &payPassword,
									const std::string &phrasePassword);

			virtual void restoreSubWallets(const std::vector<CoinInfo> &coinInfoList);

			bool importFromMnemonic(const std::string &mnemonic,
									const std::string &phrasePassword,
									const std::string &payPassword);

			bool exportKeyStore(const std::string &backupPassword,
								const std::string &payPassword,
								const std::string &keystorePath);

			bool exportMnemonic(const std::string &payPassword,
								std::string &mnemonic);

			bool initFromEntropy(const UInt128 &entropy,
								 const std::string &phrasePassword,
								 const std::string &payPassword);

			bool initFromPhrase(const std::string &phrase,
								const std::string &phrasePassword,
								const std::string &payPassword);

			Key deriveKey(const std::string &payPassword);

			UInt512 deriveSeed(const std::string &payPassword);

			void initPublicKey(const std::string &payPassword);

			SubWallet *SubWalletFactoryMethod(const CoinInfo &info,
											  const ChainParams &chainParams,
											  const std::string &payPassword,
											  MasterWallet *parent);

			void resetMnemonic(const std::string &language);

			virtual void startPeerManager(SubWallet *wallet);

			virtual void stopPeerManager(SubWallet *wallet);

		protected:
			bool _initialized;
			WalletMap _createdWallets;

			MasterWalletStore _localStore;
			KeyStore _keyStore;
			boost::shared_ptr<Mnemonic> _mnemonic;

			std::string _publicKey;
			std::string _id;

			boost::shared_ptr<IdAgentImpl> _idAgentImpl;
		};

	}
}

#endif //__ELASTOS_SDK_MASTERWALLET_H__
