-- Copyright (c) 2017-2019 The Elastos Foundation
-- Use of this source code is governed by an MIT
-- license that can be found in the LICENSE file.
-- 

local m = require("api")

--local tx_util = dofile("common.lua")
--local wallet = tx_util.wallet
local keystore = getWallet()
local password = getPassword()

if keystore == "" then
    keystore = "keystore.dat"
end
if password == "" then
    password = "123"
end

local wallet = client.new(keystore, password, false)
-- client: path, password, if create
--local wallet = client.new("keystore.dat", "123", false)

-- account
local addr = wallet:get_address()
local SponsorPublicKey = wallet:get_publickey()
print(addr)
print(SponsorPublicKey)

-- asset_id
local asset_id = m.get_asset_id()

-- amount, fee
-- local amount = 5000
--local fee = 0.001

-- deposit params
--local cr_publickey = getPublicKey()

proposal_hash = getProposalHash()
--vote_result = getVoteResult()
--code = getCode()

local fee = getFee()
local stage = getStage()

--sign = getSign()

print(proposal_hash)
print(fee)
print(stage)


local crcproposalwithdraw_payload =crcproposalwithdraw.new(proposal_hash,
        SponsorPublicKey, stage, fee , wallet)
--print(crcproposalreview_payload:get())

-- transaction: version, txType, payloadVersion, payload, locktime
local tx = transaction.new(9, 0x28, 0, crcproposalwithdraw_payload, 0)
print(tx:get())

-- input: from, amount + fee
local charge = tx:appendenough(addr, fee * 100000000)
print(charge)

-- outputpayload
local default_output = defaultoutput.new()

-- output: asset_id, value, recipient, output_paload_type, outputpaload
local charge_output = output.new(asset_id, charge, addr, 0, default_output)
tx:appendtxout(charge_output)


-- sign
tx:sign(wallet)
print(tx:get())

-- send
local hash = tx:hash()
local res = m.send_tx(tx)

print("sending " .. hash)

if (res ~= hash)
then
    print(res)
else
    print("tx send success")
end
