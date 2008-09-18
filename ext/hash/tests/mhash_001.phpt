--TEST--
mhash() test
--INI--
magic_quotes_runtime=0
--SKIPIF--
<?php
	include "skip_mhash.inc";
?>
--FILE--
<?php

$supported_hash_al = array(
"MHASH_MD5"       => "2d9bdb91f94e96d9c4e2ae532acc936a",
"MHASH_SHA1"      => "2f9341e55a9083edf5497bf83ba3db812a7de0a3",
"MHASH_HAVAL256"  => "b255feff01ad641b27358dc7909bc695a1fca53bddfdfaf19020b275928793af",
"MHASH_HAVAL192"  => "4ce837de481e1e30092ab2c610057094c988dfd7db1e01cd",
"MHASH_HAVAL224"  => "5362d1856752bf2c139bb2d6fdd772b9c515c8ce5ec82695264b85e1",
"MHASH_HAVAL160"  => "c6b36f87750b18576981bc17b4f22271947bf9cb",
"MHASH_RIPEMD160" => "6c47435aa1d359c4b7c6af46349f0c3e1258583d",
"MHASH_GOST"      => "101b0a2552cebdf5137cadf15147f21e55b6432935bb9c2c03c7e28d188b2d9e",
"MHASH_TIGER"     => "fdb9019a79c33a95677e2097abae91eb0de00b3054bb5c39",
"MHASH_CRC32"     => "83041db8",
"MHASH_CRC32B"    => "df5ab7a4"
);

$data = "This is the test of the mhash extension...";

foreach ($supported_hash_al as $hash=>$wanted) {
	$result = mhash(constant($hash), $data);
	if (bin2hex($result)==$wanted) {
		echo "$hash\nok\n";
	} else {
		echo "$hash: ";
		var_dump($wanted);
		echo "$hash: ";
		var_dump(bin2hex($result));
	}
	echo "\n";
}
?>
--EXPECT--
MHASH_MD5
ok

MHASH_SHA1
ok

MHASH_HAVAL256
ok

MHASH_HAVAL192
ok

MHASH_HAVAL224
ok

MHASH_HAVAL160
ok

MHASH_RIPEMD160
ok

MHASH_GOST
ok

MHASH_TIGER
ok

MHASH_CRC32
ok

MHASH_CRC32B
ok
