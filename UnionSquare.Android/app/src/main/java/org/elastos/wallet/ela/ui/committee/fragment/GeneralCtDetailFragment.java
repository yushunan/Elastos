package org.elastos.wallet.ela.ui.committee.fragment;

import android.annotation.SuppressLint;
import android.content.Intent;
import android.os.Bundle;
import android.support.v7.widget.AppCompatImageView;
import android.support.v7.widget.LinearLayoutManager;
import android.support.v7.widget.RecyclerView;
import android.support.v7.widget.Toolbar;
import android.view.View;
import android.widget.TextView;

import org.elastos.wallet.R;
import org.elastos.wallet.ela.ElaWallet.MyWallet;
import org.elastos.wallet.ela.base.BaseFragment;
import org.elastos.wallet.ela.bean.BusEvent;
import org.elastos.wallet.ela.db.RealmUtil;
import org.elastos.wallet.ela.db.table.Wallet;
import org.elastos.wallet.ela.rxjavahelp.BaseEntity;
import org.elastos.wallet.ela.rxjavahelp.NewBaseViewData;
import org.elastos.wallet.ela.ui.Assets.activity.TransferActivity;
import org.elastos.wallet.ela.ui.Assets.bean.BalanceEntity;
import org.elastos.wallet.ela.ui.Assets.presenter.CommonGetBalancePresenter;
import org.elastos.wallet.ela.ui.Assets.viewdata.CommonBalanceViewData;
import org.elastos.wallet.ela.ui.committee.adaper.CtExpRecAdapter;
import org.elastos.wallet.ela.ui.committee.bean.CtDetailBean;
import org.elastos.wallet.ela.ui.committee.presenter.CtDetailPresenter;
import org.elastos.wallet.ela.ui.common.bean.CommmonStringEntity;
import org.elastos.wallet.ela.ui.crvote.bean.CRListBean;
import org.elastos.wallet.ela.ui.crvote.presenter.CRlistPresenter;
import org.elastos.wallet.ela.ui.vote.ElectoralAffairs.VoteListPresenter;
import org.elastos.wallet.ela.ui.vote.activity.VoteActivity;
import org.elastos.wallet.ela.ui.vote.bean.VoteListBean;
import org.elastos.wallet.ela.utils.AppUtlis;
import org.elastos.wallet.ela.utils.Arith;
import org.elastos.wallet.ela.utils.Constant;
import org.elastos.wallet.ela.utils.DialogUtil;
import org.elastos.wallet.ela.utils.NumberiUtil;
import org.elastos.wallet.ela.utils.RxEnum;
import org.elastos.wallet.ela.utils.listener.WarmPromptListener;
import org.elastos.wallet.ela.utils.svg.GlideApp;
import org.elastos.wallet.ela.utils.view.CircleProgressView;
import org.greenrobot.eventbus.EventBus;
import org.greenrobot.eventbus.Subscribe;
import org.greenrobot.eventbus.ThreadMode;
import org.json.JSONArray;
import org.json.JSONObject;

import java.math.BigDecimal;
import java.util.ArrayList;
import java.util.List;

import butterknife.BindView;
import butterknife.OnClick;

/**
 * general committee detail
 */
public class GeneralCtDetailFragment extends BaseFragment implements NewBaseViewData, CommonBalanceViewData {
    @BindView(R.id.toolbar_title)
    TextView toolbarTitle;
    @BindView(R.id.toolbar)
    Toolbar toolbar;
    @BindView(R.id.ct_tv)
    TextView ctTitleTv;
    @BindView(R.id.line1)
    View line1;
    @BindView(R.id.experience_tv)
    TextView experienceTitleTv;
    @BindView(R.id.line2)
    View line2;
    @BindView(R.id.personal_info)
    View personalInfo;
    @BindView(R.id.work_experience)
    View workExperience;
    @BindView(R.id.recyclerview)
    RecyclerView recyclerView;
    @BindView(R.id.no_info)
    TextView norecord;
    CtExpRecAdapter adapter;
    List<CtDetailBean.Term> list = null;

    private RealmUtil realmUtil = new RealmUtil();
    private Wallet wallet = realmUtil.queryDefauleWallet();
    CtDetailPresenter presenter;

    private String id;
    private String did;

    @Override
    protected void setExtraData(Bundle data) {
        super.setExtraData(data);
        id = data.getString("id");
        did = data.getString("did");
    }

    @Override
    protected int getLayoutId() {
        return R.layout.fragment_ct_general_detail;
    }

    @Override
    protected void initView(View view) {
        registReceiver();
        setToobar(toolbar, toolbarTitle, getContext().getString(R.string.ctdetail));
        presenter = new CtDetailPresenter();
        presenter.getCouncilInfo(this, id, did);
        experienceTitleTv.setText(String.format(getString(R.string.performancerecord), String.valueOf(0)));
    }

    private void selectDetail() {
        line1.setVisibility(View.VISIBLE);
        line2.setVisibility(View.GONE);
        ctTitleTv.setTextColor(getResources().getColor(R.color.whiter));
        experienceTitleTv.setTextColor(getResources().getColor(R.color.whiter50));
        personalInfo.setVisibility(View.VISIBLE);
        workExperience.setVisibility(View.GONE);
    }

    private void selectExperience() {
        line1.setVisibility(View.GONE);
        line2.setVisibility(View.VISIBLE);
        ctTitleTv.setTextColor(getResources().getColor(R.color.whiter50));
        experienceTitleTv.setTextColor(getResources().getColor(R.color.whiter));
        personalInfo.setVisibility(View.GONE);
        workExperience.setVisibility(View.VISIBLE);
    }

    private void setRecyclerView(List<CtDetailBean.Term> datas) {
        if(datas==null || datas.size()<=0) return;
        norecord.setVisibility(View.GONE);
        recyclerView.setVisibility(View.VISIBLE);
        if (adapter == null) {
            list = new ArrayList<>();
            list.clear();
            list.addAll(datas);

            adapter = new CtExpRecAdapter(getContext(), list);
            recyclerView.setLayoutManager(new LinearLayoutManager(getContext(), LinearLayoutManager.VERTICAL, false));
            recyclerView.setAdapter(adapter);
            adapter.setCommonRvListener((position, o) -> {
                start(GeneralCtDetailFragment.class);
            });
        } else {
            list.clear();
            list.addAll(datas);
            adapter.notifyDataSetChanged();
        }
        experienceTitleTv.setText(String.format(getString(R.string.performancerecord), String.valueOf(list.size())));
    }

    JSONArray otherUnActiveVote;
    @OnClick({R.id.tab1, R.id.tab2, R.id.impeachment_btn})
    public void onClick(View view) {
        switch (view.getId()) {
            case R.id.tab1:
                selectDetail();
                break;
            case R.id.tab2:
                selectExperience();
                break;
            case R.id.impeachment_btn:
                if (otherUnActiveVote == null) {
                    otherUnActiveVote = new JSONArray();
                }
                new VoteListPresenter().getDepositVoteList("1", "all", this, false);
                new CRlistPresenter().getCRlist(1, 1000, "all", this, false);
                new CommonGetBalancePresenter().getBalance(wallet.getWalletId(), MyWallet.ELA, 2, this);
                break;
        }

    }

    String cid;
    @Override
    public void onGetData(String methodName, BaseEntity baseEntity, Object o) {
        switch (methodName) {
            case "getCouncilInfo":
                setBaseInfo((CtDetailBean) baseEntity);
                setCtInfo((CtDetailBean) baseEntity);
                setCtRecord((CtDetailBean) baseEntity);
                break;
            case "getVoteInfo":
                try {
                    String voteInfo = ((CommmonStringEntity) baseEntity).getData();
                    JSONObject voteJson = presenter.conversVote(voteInfo);
                    if ("CRC".equals(o)) {
                        otherUnActiveVote.put(presenter.getCRLastVote(voteJson));
                    } else if ("CRCImpeachment".equals(o)) {
                        String amount = Arith.mulRemoveZero(num, MyWallet.RATE_S).toPlainString();
                        JSONObject newVotes = new JSONObject();
                        newVotes.put(cid, amount);
                        presenter.createImpeachmentCRCTransaction(wallet.getWalletId(), MyWallet.ELA, "", newVotes.toString(), "", otherUnActiveVote.toString(), this);
                    }

                } catch (Exception e) {
                    e.printStackTrace();
                }
                break;
            case "getCRlist":
                List<CRListBean.DataBean.ResultBean.CrcandidatesinfoBean> crList = ((CRListBean) baseEntity).getData().getResult().getCrcandidatesinfo();
                if (crList != null && crList.size() > 0) {
                    otherUnActiveVote.put(presenter.getCRUnactiveData(crList));
                } else {
                    presenter.getVoteInfo(wallet.getWalletId(), "CRC", this);
                }
                break;
            case "getDepositVoteList":
                List<VoteListBean.DataBean.ResultBean.ProducersBean> depositList = ((VoteListBean) baseEntity).getData().getResult().getProducers();
                if (depositList != null && depositList.size() > 0) {
                    otherUnActiveVote.put(presenter.getDepositUnactiveData(depositList));
                }
                break;
            case "createImpeachmentCRCTransaction":
                goTransferActivity(((CommmonStringEntity) baseEntity).getData());
                break;

        }
    }

    private void goTransferActivity(String attributesJson) {
        Intent intent = new Intent(getActivity(), TransferActivity.class);
        intent.putExtra("amount", num);
        intent.putExtra("wallet", wallet);
        intent.putExtra("chainId", MyWallet.ELA);
        intent.putExtra("attributes", attributesJson);
        intent.putExtra("type", Constant.IMPEACHMENTCRC);
        intent.putExtra("transType", 1004);
        startActivity(intent);
    }

    @BindView(R.id.name)
    TextView name;
    @BindView(R.id.head_ic)
    AppCompatImageView headIc;
    @BindView(R.id.location)
    TextView location;
    @BindView(R.id.vote_count)
    TextView currentVotes;
    @BindView(R.id.impeachment_count)
    TextView impeachmentCount;
    @BindView(R.id.progress)
    CircleProgressView progress;
    private void setBaseInfo(CtDetailBean ctDetailBean) {
        CtDetailBean.DataBean dataBean = ctDetailBean.getData();
        name.setText(dataBean.getDidName());
        cid = dataBean.getCid();
        GlideApp.with(getContext()).load(dataBean.getAvatar()).error(R.mipmap.icon_ela).circleCrop().into(headIc);
        location.setText(AppUtlis.getLoc(getContext(), String.valueOf(dataBean.getLocation())));
//        BigDecimal gress = new BigDecimal(dataBean.getImpeachmentVotes()).divide(new BigDecimal(dataBean.getImpeachmentThroughVotes())).multiply(new BigDecimal(100));
        progress.setProgress(/*gress.floatValue()*/dataBean.getImpeachmentRatio());
        currentVotes.setText(String.valueOf(dataBean.getImpeachmentVotes()));
        impeachmentCount.setText(String.valueOf(dataBean.getImpeachmentThroughVotes()));
    }

    @BindView(R.id.did)
    TextView didTv;
    @BindView(R.id.website)
    TextView website;
    @BindView(R.id.introduction)
    TextView introduction;
    private void setCtInfo(CtDetailBean ctDetailBean) {
        if(null == ctDetailBean) return;
        CtDetailBean.DataBean dataBean = ctDetailBean.getData();
        didTv.setText(dataBean.getDid());
        if(AppUtlis.isNullOrEmpty(dataBean.getDid())) {
            didTitle.setVisibility(View.GONE);
            didTv.setVisibility(View.GONE);
        }
        website.setText(dataBean.getAddress());
        if(AppUtlis.isNullOrEmpty(dataBean.getAddress())) {
            website.setVisibility(View.GONE);
            websiteTitle.setVisibility(View.GONE);
        }
        introduction.setText(dataBean.getIntroduction());
        if(AppUtlis.isNullOrEmpty(dataBean.getIntroduction())) {
            introduction.setVisibility(View.GONE);
            introducetionTitle.setVisibility(View.GONE);
        }
    }

    @SuppressLint("DefaultLocale")
    private void setCtRecord(CtDetailBean ctDetailBean) {
        if(null == ctDetailBean) return;
        List<CtDetailBean.Term> terms = ctDetailBean.getData().getTerm();
        setRecyclerView(terms);
    }

    @BindView(R.id.did_title)
    TextView didTitle;
    @BindView(R.id.website_title)
    TextView websiteTitle;
    @BindView(R.id.introduction_title)
    TextView introducetionTitle;

    private String maxBalance;
    @Override
    public void onBalance(BalanceEntity data) {
        Intent intent = new Intent(getContext(), VoteActivity.class);
        BigDecimal balance = Arith.div(Arith.sub(data.getBalance(), 1000000), MyWallet.RATE_S, 8);
        maxBalance = NumberiUtil.removeZero(balance.toPlainString());

        if ((balance.compareTo(new BigDecimal(0)) <= 0)) {
            maxBalance = "0";
            intent.putExtra("maxBalance", "0");
        } else {
            intent.putExtra("maxBalance", maxBalance);
        }
        intent.putExtra("type", Constant.IMPEACHMENTCRC);
        startActivity(intent);
    }

    String num;//弹劾票数
    @Subscribe(threadMode = ThreadMode.MAIN)
    public void Event(BusEvent result) {
        int integer = result.getCode();
        if (integer == RxEnum.VOTETRANSFERACTIVITY.ordinal()) {
            num = result.getName();
            presenter.getVoteInfo(wallet.getWalletId(), "CRCImpeachment", this);
        }
        if (integer == RxEnum.TRANSFERSUCESS.ordinal()) {
            new DialogUtil().showTransferSucess(getBaseActivity(), new WarmPromptListener() {
                @Override
                public void affireBtnClick(View view) {
                    popBackFragment();
                }
            });

        }

    }

    public void registReceiver() {
        if (!EventBus.getDefault().isRegistered(this))
            EventBus.getDefault().register(this);
    }
}
