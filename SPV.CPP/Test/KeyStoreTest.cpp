// Copyright (c) 2012-2018 The Elastos Open Source Project
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#define CATCH_CONFIG_MAIN

#include <fstream>
#include <catch.hpp>
#include <boost/filesystem.hpp>

#include "KeyStore/KeyStore.h"

using namespace Elastos::SDK;

TEST_CASE("save/open", "[KeyStore]") {
	SECTION("save") {
		// comment real information, instead of test info into str_ss on KeyStore::save
		KeyStore ks;
		const boost::filesystem::path path = "conf.json";
		std::string password = "password";

		REQUIRE(true == ks.save(path, password));
	}

	SECTION("open") {
		KeyStore ks;
		const boost::filesystem::path path = "conf.json";
		std::string password = "password";

		REQUIRE(true == ks.open(path, password));
	}
}