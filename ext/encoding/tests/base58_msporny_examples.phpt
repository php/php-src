--TEST--
Base58 draft-msporny-base58 test vectors
--EXTENSIONS--
encoding
--FILE--
<?php

use function Encoding\base58_encode;
use function Encoding\base58_decode;

// Test vector 1: "Hello World!"
echo base58_encode("Hello World!");
echo "\n";

// Test vector 2: "The quick brown fox jumps over the lazy dog."
echo base58_encode("The quick brown fox jumps over the lazy dog.");
echo "\n";

// Test vector 3: 0x0000287fb4cd
echo base58_encode("\x00\x00\x28\x7f\xb4\xcd");
echo "\n";

// Round-trip decode each vector
var_dump(base58_decode("2NEpo7TZRRrLZSi2U") === "Hello World!");

var_dump(base58_decode("USm3fpXnKG5EUBx2ndxBDMPVciP5hGey2Jh4NDv6gmeo1LkMeiKrLJUUBk6Z") === "The quick brown fox jumps over the lazy dog.");

var_dump(base58_decode("11233QC4") === "\x00\x00\x28\x7f\xb4\xcd");

?>
--EXPECT--
2NEpo7TZRRrLZSi2U
USm3fpXnKG5EUBx2ndxBDMPVciP5hGey2Jh4NDv6gmeo1LkMeiKrLJUUBk6Z
11233QC4
bool(true)
bool(true)
bool(true)
