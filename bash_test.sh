#!/bin/bash

# the super secret preimage
PREIMAGE="jmjatlanta"
# how long the contract is good for
TIMEOUT="5 days"
# who will receive the funds. Note: this is a public key, not a Bitcoin address.
SENDER_PK="02a67b6a0bb3e2724582c3316f137982b0fd1cd7fa778c49d1428da4bb47d83957"
# who will get the funds back after the timeout (this is a public key, not a Bitcoin address)
# obtain the public key from your bitcoin address by doing the bitcoin-cli command getaddressinfo your_public_address
RECEIVER_PK="02a67b6a0bb3e2724582c3316f137982b0fd1cd7fa778c49d1428da4bb47d83957"

#This will create the contract
HASHLOCK="$(./hash_160 text ${PREIMAGE})"
#TIMELOCK="$(./calc_timeout ${TIMEOUT})"
TIMELOCK=1554348732
SCRIPT_ADDRESS="$(./calc_script_address testnet ${HASHLOCK} ${RECEIVER_PK} ${TIMELOCK} ${SENDER_PK})"
REDEEM_SCRIPT="$(./calc_redeem_script testnet ${HASHLOCK} ${RECEIVER_PK} ${TIMELOCK} ${SENDER_PK})"
echo "The timeout was ${TIMELOCK}"
echo "The script address is ${SCRIPT_ADDRESS}"
echo "The redeem script is ${REDEEM_SCRIPT}"

# fund the script (you must do this manually, 
# then get the utxo_txid and utxo_vout for the next step)
# see bash_test2.sh for more information
#
# $BITCOIN_CLI sendtoaddress $SCRIPT_ADDRESS $FUNDING_AMOUNT
# $BITCOIN_CLI generate 1


