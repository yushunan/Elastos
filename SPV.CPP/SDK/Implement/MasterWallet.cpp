// Copyright (c) 2012-2018 The Elastos Open Source Project
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <boost/filesystem.hpp>
#include <Core/BRKey.h>

#include "BRBase58.h"
#include "BRBIP39Mnemonic.h"

#include "Utils.h"
#include "MasterPubKey.h"
#include "MasterWallet.h"
#include "SubWallet.h"
#include "IdChainSubWallet.h"
#include "MainchainSubWallet.h"
#include "SidechainSubWallet.h"
#include "Log.h"
#include "Config.h"
#include "ParamChecker.h"
#include "BigIntFormat.h"
#include "Wallet_Tool.h"
#include "BTCBase58.h"

#define MNEMONIC_FILE_PREFIX "mnemonic_"
#define MNEMONIC_FILE_EXTENSION ".txt"


namespace fs = boost::filesystem;

namespace Elastos {
	namespace SDK {

		MasterWallet::MasterWallet(const std::string &language) :
			_initialized(false),
			_dbRoot("Data") {

			ParamChecker::checkNotEmpty(language);

			resetMnemonic(language);
			_keyStore.json().setMnemonicLanguage(language);
		}

		MasterWallet::MasterWallet(const std::string &phrasePassword,
								   const std::string &payPassword,
								   const std::string &language,
								   const std::string &rootPath) :
			_initialized(true),
			_dbRoot(rootPath) {

			ParamChecker::checkPasswordWithNullLegal(phrasePassword);
			ParamChecker::checkPassword(payPassword);
			ParamChecker::checkNotEmpty(language);
			ParamChecker::checkNotEmpty(rootPath);

			resetMnemonic(language);

			CMemBlock<uint8_t> seed128 = Wallet_Tool::GenerateSeed128();
#ifndef MNEMONIC_SOURCE_H
			CMemBlock<char> phrase = Wallet_Tool::GeneratePhraseFromSeed(seed128, _mnemonic->words());
#else
			CMemBlock<char> phrase = Wallet_Tool::GeneratePhraseFromSeed(seed128, language);
#endif
			std::string str_phrase = (const char *) phrase;
			initFromPhrase(str_phrase, phrasePassword, payPassword);
		}

		MasterWallet::~MasterWallet() {

		}

		ISubWallet *
		MasterWallet::CreateSubWallet(SubWalletType type, const std::string &chainID, uint32_t coinTypeIndex,
									  const std::string &payPassword, bool singleAddress, uint64_t feePerKb) {

			if (!Initialized())
				throw std::logic_error("Current master wallet is not initialized.");

			ParamChecker::checkNotEmpty(chainID);

			if (chainID.size() > 128)
				throw std::invalid_argument("Chain id should less than 128.");

			ParamChecker::checkPassword(payPassword);

			//todo limit coinTypeIndex and feePerKb if needed in future

			if (_createdWallets.find(chainID) != _createdWallets.end()) {
				deriveKey(payPassword);
				return _createdWallets[chainID];
			}

			CoinInfo info;
			info.setWalletType(type);
			info.setEaliestPeerTime(0);
			info.setIndex(coinTypeIndex);
			info.setSingleAddress(singleAddress);
			info.setUsedMaxAddressIndex(0);
			info.setChainId(chainID);
			info.setFeePerKb(feePerKb);
			SubWallet *subWallet = SubWalletFactoryMethod(info, ChainParams::mainNet(), payPassword, this);
			_createdWallets[chainID] = subWallet;
			startPeerManager(subWallet);
			return subWallet;
		}

		ISubWallet *
		MasterWallet::RecoverSubWallet(SubWalletType type, const std::string &chainID, uint32_t coinTypeIndex,
									   const std::string &payPassword, bool singleAddress, uint32_t limitGap,
									   uint64_t feePerKb) {
			if (!Initialized())
				throw std::logic_error("Current master wallet is not initialized.");

			if (_createdWallets.find(chainID) != _createdWallets.end())
				return _createdWallets[chainID];

			if (limitGap > SEQUENCE_GAP_LIMIT_EXTERNAL) {
				throw std::invalid_argument("Limit gap should less than or equal 10.");
			}

			ISubWallet *subWallet = CreateSubWallet(type, chainID, coinTypeIndex, payPassword, singleAddress, feePerKb);
			SubWallet *walletInner = dynamic_cast<SubWallet *>(subWallet);
			assert(walletInner != nullptr);
			walletInner->recover(limitGap);

			_createdWallets[chainID] = subWallet;
			return subWallet;
		}

		void MasterWallet::DestroyWallet(ISubWallet *wallet) {

			if (!Initialized())
				throw std::logic_error("Current master wallet is not initialized.");

			if (wallet == nullptr)
				throw std::invalid_argument("Sub wallet can't be null.");

			if (_createdWallets.empty())
				throw std::logic_error("There is no sub wallet in this wallet.");

			if (std::find_if(_createdWallets.begin(), _createdWallets.end(),
							 [wallet](const WalletMap::value_type &item) {
								 return item.second == wallet;
							 }) == _createdWallets.end())
				throw std::logic_error("Specified sub wallet is not belong to current master wallet.");

			_createdWallets.erase(std::find_if(_createdWallets.begin(), _createdWallets.end(),
											   [wallet](const WalletMap::value_type &item) {
												   return item.second == wallet;
											   }));
			SubWallet *walletInner = dynamic_cast<SubWallet *>(wallet);
			assert(walletInner != nullptr);
			stopPeerManager(walletInner);
			delete walletInner;
		}

		std::string MasterWallet::GetPublicKey() {
			if (!Initialized())
				throw std::logic_error("Current master wallet is not initialized.");

			return _publicKey;
		}

		bool MasterWallet::importFromKeyStore(const std::string &keystorePath, const std::string &backupPassword,
											  const std::string &payPassword, const std::string &phrasePassword,
											  const std::string &rootPath) {
			ParamChecker::checkNotEmpty(keystorePath);
			ParamChecker::checkPassword(backupPassword);
			ParamChecker::checkPassword(payPassword);
			ParamChecker::checkPasswordWithNullLegal(phrasePassword);
			ParamChecker::checkNotEmpty(rootPath);

			_dbRoot = rootPath;

			if (!_keyStore.open(keystorePath, backupPassword)) {
				Log::error("Import key error.");
				return false;
			}

			CMBlock phrasePass_Raw0 = Utils::convertToMemBlock<unsigned char>(
				_keyStore.json().getEncryptedPhrasePassword());
			std::string phrasePass = "";
			if (true == phrasePass_Raw0) {
				CMBlock phrasePass_Raw1(phrasePass_Raw0.GetSize() + 1);
				phrasePass_Raw1.Zero();
				memcpy(phrasePass_Raw1, phrasePass_Raw0, phrasePass_Raw0.GetSize());
				CMBlock phrasePassRaw = Utils::decrypt(phrasePass_Raw1, payPassword);
				phrasePass = Utils::convertToString(phrasePassRaw);
			}
			phrasePass = phrasePassword != "" ? phrasePassword : phrasePass;

			std::string mnemonic = _keyStore.json().getMnemonic();
			CMBlock cb_mnemonic;
			cb_mnemonic.SetMemFixed((const uint8_t *) mnemonic.c_str(), mnemonic.size());
			_encryptedMnemonic = Utils::encrypt(cb_mnemonic, payPassword);
			resetMnemonic(_keyStore.json().getMnemonicLanguage());

			return initFromPhrase(mnemonic, phrasePass, payPassword);
		}

		bool MasterWallet::importFromMnemonic(const std::string &mnemonic, const std::string &phrasePassword,
											  const std::string &payPassword, const std::string &rootPath) {
			ParamChecker::checkNotEmpty(mnemonic);
			ParamChecker::checkPassword(payPassword);
			ParamChecker::checkPasswordWithNullLegal(phrasePassword);
			ParamChecker::checkNotEmpty(rootPath);

			_dbRoot = rootPath;

			CMemBlock<char> cb_mnemonic;
			cb_mnemonic.SetMemFixed(mnemonic.c_str(), mnemonic.size() + 1);
#ifdef MNEMONIC_SOURCE_H
			if (!Wallet_Tool::PhraseIsValid(cb_mnemonic, _mnemonic->getLanguage())) {
#else
			if (!Wallet_Tool::PhraseIsValid(cb_mnemonic, _mnemonic->words())) {
#endif
				Log::error("Invalid mnemonic.");
				return false;
			}

			return initFromPhrase(mnemonic, phrasePassword, payPassword);
		}

		bool MasterWallet::exportKeyStore(const std::string &backupPassword, const std::string &payPassword,
										  const std::string &keystorePath) {
			if (_keyStore.json().getEncryptedPhrasePassword().empty())
				_keyStore.json().setEncryptedPhrasePassword((const char *) (unsigned char *) _encryptedPhrasePass);
			CMBlock cb_Mnemonic = Utils::decrypt(_encryptedMnemonic, payPassword);
			if (false == cb_Mnemonic) {
				return false;
			}
			CMemBlock<char> cb_char_Mnemonic(cb_Mnemonic.GetSize() + 1);
			cb_char_Mnemonic.Zero();
			memcpy(cb_char_Mnemonic, cb_Mnemonic, cb_Mnemonic.GetSize());
			if (_keyStore.json().getMnemonic().empty())
				_keyStore.json().setMnemonic((const char *) cb_char_Mnemonic);

			_keyStore.json().clearCoinInfo();
			std::for_each(_createdWallets.begin(), _createdWallets.end(), [this](const WalletMap::value_type &item) {
				SubWallet *subWallet = dynamic_cast<SubWallet *>(item.second);
				_keyStore.json().addCoinInfo(subWallet->_info);
			});

			if (!_keyStore.save(keystorePath, backupPassword)) {
				Log::error("Export key error.");
				return false;
			}

			return true;
		}

		bool MasterWallet::exportMnemonic(const std::string &payPassword, std::string &mnemonic) {

			CMBlock cb_Mnemonic = Utils::decrypt(_encryptedMnemonic, payPassword);
			if (false == cb_Mnemonic) {
				return false;
			}
			CMemBlock<char> cb_char_Mnemonic(cb_Mnemonic.GetSize() + 1);
			cb_char_Mnemonic.Zero();
			memcpy(cb_char_Mnemonic, cb_Mnemonic, cb_Mnemonic.GetSize());
			mnemonic = (const char *) cb_char_Mnemonic;
			return true;
		}

		bool MasterWallet::Initialized() const {
			return _initialized;
		}

		bool MasterWallet::initFromEntropy(const UInt128 &entropy, const std::string &phrasePassword,
										   const std::string &payPassword) {

			std::string phrase = MasterPubKey::generatePaperKey(entropy, _mnemonic->words());
			return initFromPhrase(phrase, phrasePassword, payPassword);
		}

		bool MasterWallet::initFromPhrase(const std::string &phrase, const std::string &phrasePassword,
										  const std::string &payPassword) {
			assert(phrase.size() > 0);
			CMemBlock<char> cb_phrase_;
			cb_phrase_.SetMemFixed(phrase.c_str(), phrase.size() + 1);
			std::string language = _keyStore.json().getMnemonicLanguage();
			std::string _language = _mnemonic->getLanguage();
			if (language != "" && language != _language) {
				resetMnemonic(language);
			}
#ifdef MNEMONIC_SOURCE_H
			if (!Wallet_Tool::PhraseIsValid(cb_phrase_, language)) {
				if (!Wallet_Tool::PhraseIsValid(cb_phrase_, "chinese")) {
					Log::error("Phrase is unvalid.");
					return false;
				} else {
					_keyStore.json().setMnemonicLanguage("chinese");
				}
			}
#else
			if (!Wallet_Tool::PhraseIsValid(cb_phrase_, _mnemonic->words())) {
				resetMnemonic("chinese");
				if (!Wallet_Tool::PhraseIsValid(cb_phrase_, _mnemonic->words())) {
					Log::error("Phrase is unvalid.");
					return false;
				}
			}
#endif
			CMBlock cb_phrase0 = Utils::convertToMemBlock<unsigned char>(phrase);
			_encryptedMnemonic = Utils::encrypt(cb_phrase0, payPassword);
			CMBlock PhrasePass = Utils::convertToMemBlock<unsigned char>(phrasePassword);
			_encryptedPhrasePass = Utils::encrypt(PhrasePass, payPassword);

			//init master public key and private key
			CMemBlock<char> cb_phrase;
			cb_phrase.SetMemFixed(phrase.c_str(), phrase.size() + 1);
			std::string prikey_base58 = Wallet_Tool::getDeriveKey_base58(cb_phrase, phrasePassword);
			CMemBlock<unsigned char> prikey = BTCBase58::DecodeBase58(prikey_base58);

			CMBlock cb_tmp;
			cb_tmp.SetMemFixed((const uint8_t *) (void *) prikey, prikey.GetSize());
			CMBlock privKey = Key::getAuthPrivKeyForAPI(cb_tmp);

			_encryptedKey = Utils::encrypt(privKey, payPassword);
			initPublicKey(payPassword);

			_initialized = true;
			return true;
		}

		Key MasterWallet::deriveKey(const std::string &payPassword) {
			CMBlock keyData = Utils::decrypt(_encryptedKey, payPassword);
			ParamChecker::checkDataNotEmpty(keyData);

			Key key;
			char stmp[keyData.GetSize()];
			memcpy(stmp, keyData, keyData.GetSize());
			std::string secret(stmp, keyData.GetSize());
			key.setPrivKey(secret);
			return key;
		}

		void MasterWallet::initPublicKey(const std::string &payPassword) {
			Key key = deriveKey(payPassword);
			//todo throw exception here
			if (key.getPrivKey() == "") {
				return;
			}
			size_t len = BRKeyPubKey(key.getRaw(), nullptr, 0);
			uint8_t pubKey[len];
			BRKeyPubKey(key.getRaw(), pubKey, len);
			CMBlock data;
			data.SetMemFixed(pubKey, len);
			_publicKey = Key::encodeHex(data);
		}

		std::string MasterWallet::Sign(const std::string &message, const std::string &payPassword) {
			if (!Initialized())
				throw std::logic_error("Current master wallet is not initialized.");

			ParamChecker::checkNotEmpty(message);
			ParamChecker::checkPassword(payPassword);

			Key key = deriveKey(payPassword);

			UInt256 md;
			BRSHA256(&md, message.c_str(), message.size());

			CMBlock signedData = key.sign(md);

			char data[signedData.GetSize()];
			memcpy(data, signedData, signedData.GetSize());
			std::string singedMsg(data, signedData.GetSize());
			return singedMsg;
		}

		nlohmann::json
		MasterWallet::CheckSign(const std::string &publicKey, const std::string &message,
								const std::string &signature) {
			if (!Initialized())
				throw std::logic_error("Current master wallet is not initialized.");

			CMBlock signatureData(signature.size());
			memcpy(signatureData, signature.c_str(), signature.size());

			UInt256 md;
			BRSHA256(&md, message.c_str(), message.size());

			bool r = Key::verifyByPublicKey(publicKey, md, signatureData);
			nlohmann::json jsonData;
			jsonData["Result"] = r;
			return jsonData;
		}

		bool MasterWallet::IsIdValid(const std::string &id) {
			return Address::isValidIdAddress(id);
		}

		UInt512 MasterWallet::deriveSeed(const std::string &payPassword) {
			UInt512 result;
			CMBlock entropyData = Utils::decrypt(_encryptedMnemonic, payPassword);
			ParamChecker::checkDataNotEmpty(entropyData, false);

			CMemBlock<char> mnemonic(entropyData.GetSize() + 1);
			mnemonic.Zero();
			memcpy(mnemonic, entropyData, entropyData.GetSize());

			std::string phrasePassword = _encryptedPhrasePass.GetSize() == 0
										 ? ""
										 : Utils::convertToString(Utils::decrypt(_encryptedPhrasePass, payPassword));

			std::string prikey_base58 = Wallet_Tool::getDeriveKey_base58(mnemonic, phrasePassword);
			CMemBlock<uint8_t> prikey = BTCBase58::DecodeBase58(prikey_base58);
			assert(prikey.GetSize() == sizeof(result));
			memcpy(&result, prikey, prikey.GetSize());

			return result;
		}

		SubWallet *MasterWallet::SubWalletFactoryMethod(const CoinInfo &info, const ChainParams &chainParams,
														const std::string &payPassword, MasterWallet *parent) {
			switch (info.getWalletType()) {
				case Mainchain:
					return new MainchainSubWallet(info, chainParams, payPassword, parent);
				case Sidechain:
					return new SidechainSubWallet(info, chainParams, payPassword, parent);
				case Idchain:
					return new IdChainSubWallet(info, chainParams, payPassword, parent);
				case Normal:
				default:
					return new SubWallet(info, chainParams, payPassword, parent);
			}
		}

		void MasterWallet::resetMnemonic(const std::string &language) {
			_keyStore.json().setMnemonicLanguage(language);
			fs::path mnemonicPath = _dbRoot;
			_mnemonic = boost::shared_ptr<Mnemonic>(new Mnemonic(language, mnemonicPath));
		}

		bool MasterWallet::DeriveIdAndKeyForPurpose(uint32_t purpose, uint32_t index, const std::string &payPassword,
													std::string &id, std::string &key) {
			if (!Initialized())
				throw std::logic_error("Current master wallet is not initialized.");

			ParamChecker::checkPassword(payPassword);

			if (purpose == 44)
				throw std::invalid_argument("Can not use reserved purpose.");

			UInt512 seed = deriveSeed(payPassword);

			BRKey *privkey = new BRKey;
			UInt256 chainCode;
			Key::deriveKeyAndChain(privkey, chainCode, &seed, sizeof(seed), 2, purpose, index);
			Key wrappedKey(privkey);
			id = wrappedKey.keyToAddress(ELA_IDCHAIN);
			key = wrappedKey.toString();
			return true;
		}

		void MasterWallet::startPeerManager(SubWallet *wallet) {
			wallet->_walletManager->start();
		}

		void MasterWallet::stopPeerManager(SubWallet *wallet) {
			wallet->_walletManager->stop();
		}

	}
}