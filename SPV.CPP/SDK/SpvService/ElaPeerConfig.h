// Copyright (c) 2012-2018 The Elastos Open Source Project
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef __ELASTOS_SDK_ELAPEERCONFIG_H__
#define __ELASTOS_SDK_ELAPEERCONFIG_H__

#include <nlohmann/json.hpp>

namespace Elastos {
	namespace ElaWallet {

		static nlohmann::json ElaPeerConfig =
					R"(
						  {
							"MagicNumber": 20180627,
							"KnowingPeers":
							[
								{
									"Address": "35.154.129.25",
									"Port": 22866,
									"Timestamp": 0,
									"Services": 1,
									"Flags": 0
								},
								{
									"Address": "13.124.164.153",
									"Port": 22866,
									"Timestamp": 0,
									"Services": 1,
									"Flags": 0
								},
								{
									"Address": "18.179.168.103",
									"Port": 22866,
									"Timestamp": 0,
									"Services": 1,
									"Flags": 0
								}
							]
						}
					)"_json;

	}
}

#endif //__ELASTOS_SDK_ELAPEERCONFIG_H__
