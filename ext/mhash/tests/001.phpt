--TEST--
mhash() test
--SKIPIF--
<?php
	include "skip.inc";
?>
--FILE--
<?php

$supported_hash_al = array(
"MHASH_MD5",
"MHASH_SHA1",
"MHASH_HAVAL256",
"MHASH_HAVAL192",
"MHASH_HAVAL224",
"MHASH_HAVAL160",
"MHASH_RIPEMD160",
"MHASH_GOST",
"MHASH_TIGER",
"MHASH_CRC32",
"MHASH_CRC32B"
);

	$data = "This is the test of the mhash extension...";

	foreach ($supported_hash_al as $hash) {
		echo $hash . "\n";
		var_dump(mhash(constant($hash), $data));
		echo "\n";
	}
?>
--EXPECT--
MHASH_MD5
string(16) "-õ€ë˘NñŸƒ‚ÆS*Ãìj"

MHASH_SHA1
string(20) "/ìAÂZêÉÌıI{¯;£€Å*}‡£"

MHASH_HAVAL256
string(32) "≤U˛ˇ≠d'5ç«êõ∆ï°¸•;›˝˙Òê ≤uíáìØ"

MHASH_HAVAL192
string(24) "LË7ﬁH0	*≤∆pî…àﬂ◊€Õ"

MHASH_HAVAL224
string(28) "Sb—ÖgRø,õ≤÷˝◊rπ≈»Œ^»&ï&KÖ·"

MHASH_HAVAL160
string(20) "∆≥oáuWiÅº¥Ú"qî{˘À"

MHASH_RIPEMD160
string(20) "lGCZ°”Yƒ∑∆ØF4ü>XX="

MHASH_GOST
string(32) "
%RŒΩı|≠ÒQGÚU∂C)5ªú,«‚çã-û"

MHASH_TIGER
string(24) "ï:√yöπ˝ÎëÆ´ó ~g9\ªT0‡
"

MHASH_CRC32
string(4) "É∏"

MHASH_CRC32B
string(4) "§∑Zﬂ"

