package core

import (
	"bytes"
	"errors"
	"io"

	. "github.com/elastos/Elastos.ELA.Utility/common"
)

const PayloadRegisterProducerVersion byte = 0x00

type PayloadRegisterProducer struct {
	PublicKey string
	NickName  string
	Url       string
	Location  uint64
	IP        string
}

func (a *PayloadRegisterProducer) Data(version byte) []byte {
	buf := new(bytes.Buffer)
	if err := a.Serialize(buf, version); err != nil {
		return []byte{0}
	}
	return buf.Bytes()
}

func (a *PayloadRegisterProducer) Serialize(w io.Writer, version byte) error {
	err := WriteVarString(w, a.PublicKey)
	if err != nil {
		return errors.New("[PayloadRegisterProducer], PublicKey serialize failed.")
	}

	err = WriteVarString(w, a.NickName)
	if err != nil {
		return errors.New("[PayloadRegisterProducer], NickName serialize failed.")
	}

	err = WriteVarString(w, a.Url)
	if err != nil {
		return errors.New("[PayloadRegisterProducer], Url serialize failed.")
	}

	err = WriteUint64(w, a.Location)
	if err != nil {
		return errors.New("[PayloadRegisterProducer], Location serialize failed.")
	}

	err = WriteVarString(w, a.IP)
	if err != nil {
		return errors.New("[PayloadRegisterProducer], IP serialize failed.")
	}
	return nil
}

func (a *PayloadRegisterProducer) Deserialize(r io.Reader, version byte) error {
	publicKey, err := ReadVarString(r)
	if err != nil {
		return errors.New("[PayloadRegisterProducer], PublicKey deserialize failed.")
	}

	nickName, err := ReadVarString(r)
	if err != nil {
		return errors.New("[PayloadRegisterProducer], NickName deserialize failed.")
	}

	url, err := ReadVarString(r)
	if err != nil {
		return errors.New("[PayloadRegisterProducer], Url deserialize failed.")
	}

	location, err := ReadUint64(r)
	if err != nil {
		return errors.New("[PayloadRegisterProducer], Location deserialize failed.")
	}

	ip, err := ReadVarString(r)
	if err != nil {
		return errors.New("[PayloadRegisterProducer], Location deserialize failed.")
	}

	a.PublicKey = publicKey
	a.NickName = nickName
	a.Url = url
	a.Location = location
	a.IP = ip

	return nil
}
