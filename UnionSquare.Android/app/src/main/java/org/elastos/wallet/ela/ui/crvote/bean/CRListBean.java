package org.elastos.wallet.ela.ui.crvote.bean;

import android.os.Parcel;
import android.os.Parcelable;
import android.support.annotation.NonNull;

import org.elastos.wallet.ela.rxjavahelp.BaseEntity;

import java.io.Serializable;
import java.math.BigDecimal;
import java.util.List;

public class CRListBean extends BaseEntity {


    @Override
    public String toString() {
        return "CRListBean{" +
                "message='" + message + '\'' +
                ", data=" + data +
                ", exceptionMsg=" + exceptionMsg +
                '}';
    }

    /**
     * message : Query successful ^_^
     * data : {"error":null,"id":null,"jsonrpc":"2.0","result":{"crcandidatesinfo":[{"code":"2102c632e27b19260d80d58a857d2acd9eb603f698445cc07ba94d52296468706331ac","did":"im4yHzAA68RRUCf8gXD6i43rJYHK9VJqLH","nickname":"raocr","url":"http://52.80.54.227:9000/","location":93,"state":"Active","votes":"0","index":0}],"totalvotes":"0","totalcounts":1}}
     * exceptionMsg : null
     */

    private String message;
    private DataBean data;
    private Object exceptionMsg;

    public String getMessage() {
        return message;
    }

    public void setMessage(String message) {
        this.message = message;
    }

    public DataBean getData() {
        return data;
    }

    public void setData(DataBean data) {
        this.data = data;
    }

    public Object getExceptionMsg() {
        return exceptionMsg;
    }

    public void setExceptionMsg(Object exceptionMsg) {
        this.exceptionMsg = exceptionMsg;
    }

    public static class DataBean {
        /**
         * error : null
         * id : null
         * jsonrpc : 2.0
         * result : {"crcandidatesinfo":[{"code":"2102c632e27b19260d80d58a857d2acd9eb603f698445cc07ba94d52296468706331ac","did":"im4yHzAA68RRUCf8gXD6i43rJYHK9VJqLH","nickname":"raocr","url":"http://52.80.54.227:9000/","location":93,"state":"Active","votes":"0","index":0}],"totalvotes":"0","totalcounts":1}
         */

        private Object error;
        private Object id;
        private String jsonrpc;
        private ResultBean result;

        public Object getError() {
            return error;
        }

        public void setError(Object error) {
            this.error = error;
        }

        public Object getId() {
            return id;
        }

        public void setId(Object id) {
            this.id = id;
        }

        public String getJsonrpc() {
            return jsonrpc;
        }

        public void setJsonrpc(String jsonrpc) {
            this.jsonrpc = jsonrpc;
        }

        public ResultBean getResult() {
            return result;
        }

        public void setResult(ResultBean result) {
            this.result = result;
        }

        public static class ResultBean implements Serializable {
            /**
             * crcandidatesinfo : [{"code":"2102c632e27b19260d80d58a857d2acd9eb603f698445cc07ba94d52296468706331ac","did":"im4yHzAA68RRUCf8gXD6i43rJYHK9VJqLH","nickname":"raocr","url":"http://52.80.54.227:9000/","location":93,"state":"Active","votes":"0","index":0}]
             * totalvotes : 0
             * totalcounts : 1
             */

            private String totalvotes;
            private int totalcounts;
            private List<CrcandidatesinfoBean> crcandidatesinfo;

            public String getTotalvotes() {
                return totalvotes;
            }

            public void setTotalvotes(String totalvotes) {
                this.totalvotes = totalvotes;
            }

            public int getTotalcounts() {
                return totalcounts;
            }

            public void setTotalcounts(int totalcounts) {
                this.totalcounts = totalcounts;
            }

            public List<CrcandidatesinfoBean> getCrcandidatesinfo() {
                return crcandidatesinfo;
            }

            public void setCrcandidatesinfo(List<CrcandidatesinfoBean> crcandidatesinfo) {
                this.crcandidatesinfo = crcandidatesinfo;
            }

            public class CrcandidatesinfoBean implements Comparable<CrcandidatesinfoBean>, Parcelable, Serializable {
                /**
                 * code : 2102c632e27b19260d80d58a857d2acd9eb603f698445cc07ba94d52296468706331ac
                 * did : im4yHzAA68RRUCf8gXD6i43rJYHK9VJqLH
                 * nickname : raocr
                 * url : http://52.80.54.227:9000/
                 * location : 93
                 * state : Active
                 * votes : 0
                 * index : 0
                 */
                @Override
                public int compareTo(@NonNull CrcandidatesinfoBean o) {
                    return Double.compare(Double.parseDouble(o.getVotes()), Double.parseDouble(this.getVotes()));

                }

                @Override
                public boolean equals(Object other) {
                    return this.getDid().equals(((CrcandidatesinfoBean) other).getDid());
                }

                private BigDecimal curentBalance;//为了购物车新增的输入控件字段
                private boolean isChecked;//为了购物车新增的控件选中字段
                private boolean isSelect;//为了列表容易展示   是否已经加入购物车
                private String code;
                private String did;
                private String nickname;
                private String url;
                private int location;
                private String state;
                private String votes;
                private int index;
                private String voterate;

                public BigDecimal getCurentBalance() {
                    return curentBalance;
                }

                public void setCurentBalance(BigDecimal curentBalance) {
                    this.curentBalance = curentBalance;
                }

                public boolean isChecked() {
                    return isChecked;
                }

                public void setChecked(boolean checked) {
                    isChecked = checked;
                }

                public boolean isSelect() {
                    return isSelect;
                }

                public void setSelect(boolean select) {
                    isSelect = select;
                }

                public String getVoterate() {
                    return voterate;
                }

                public void setVoterate(String voterate) {
                    this.voterate = voterate;
                }

                public String getCode() {
                    return code;
                }

                public void setCode(String code) {
                    this.code = code;
                }

                public String getDid() {
                    return did;
                }

                public void setDid(String did) {
                    this.did = did;
                }

                public String getNickname() {
                    return nickname;
                }

                public void setNickname(String nickname) {
                    this.nickname = nickname;
                }

                public String getUrl() {
                    return url;
                }

                public void setUrl(String url) {
                    this.url = url;
                }

                public int getLocation() {
                    return location;
                }

                public void setLocation(int location) {
                    this.location = location;
                }

                public String getState() {
                    return state;
                }

                public void setState(String state) {
                    this.state = state;
                }

                public String getVotes() {
                    return votes;
                }

                public void setVotes(String votes) {
                    this.votes = votes;
                }

                public int getIndex() {
                    return index;
                }

                public void setIndex(int index) {
                    this.index = index;
                }

                @Override
                public int describeContents() {
                    return 0;
                }

                @Override
                public void writeToParcel(Parcel dest, int flags) {
                    dest.writeSerializable(this.curentBalance);
                    dest.writeByte(this.isChecked ? (byte) 1 : (byte) 0);
                    dest.writeByte(this.isSelect ? (byte) 1 : (byte) 0);
                    dest.writeString(this.code);
                    dest.writeString(this.did);
                    dest.writeString(this.nickname);
                    dest.writeString(this.url);
                    dest.writeInt(this.location);
                    dest.writeString(this.state);
                    dest.writeString(this.votes);
                    dest.writeInt(this.index);
                    dest.writeString(this.voterate);
                }

                public CrcandidatesinfoBean() {
                }

                protected CrcandidatesinfoBean(Parcel in) {
                    this.curentBalance = (BigDecimal) in.readSerializable();
                    this.isChecked = in.readByte() != 0;
                    this.isSelect = in.readByte() != 0;
                    this.code = in.readString();
                    this.did = in.readString();
                    this.nickname = in.readString();
                    this.url = in.readString();
                    this.location = in.readInt();
                    this.state = in.readString();
                    this.votes = in.readString();
                    this.index = in.readInt();
                    this.voterate = in.readString();
                }

                public final Parcelable.Creator<CrcandidatesinfoBean> CREATOR = new Parcelable.Creator<CrcandidatesinfoBean>() {
                    @Override
                    public CrcandidatesinfoBean createFromParcel(Parcel source) {
                        return new CrcandidatesinfoBean(source);
                    }

                    @Override
                    public CrcandidatesinfoBean[] newArray(int size) {
                        return new CrcandidatesinfoBean[size];
                    }
                };
            }
        }
    }
}
