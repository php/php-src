--TEST--
mhash() test
--INI--
magic_quotes_runtime=0
--SKIPIF--
<?php
	include "skip.inc";
?>
--FILE--
<?php

$supported_hash_al = array(
"MHASH_MD5"       => "-›Û‘ùN–ÙÄâ®S*Ì“j",
"MHASH_SHA1"      => "/“AåZƒíõI{ø;£Û*}à£",
"MHASH_HAVAL256"  => "²Uþÿ­d'5Ç›Æ•¡ü¥;Ýýúñ ²u’‡“¯",
"MHASH_HAVAL192"  => "Lè7ÞH0	*²Æp”Éˆß×ÛÍ",
"MHASH_HAVAL224"  => "SbÑ…gR¿,›²Öý×r¹ÅÈÎ^È&•&K…á",
"MHASH_HAVAL160"  => "Æ³o‡uWi¼´ò\"q”{ùË",
"MHASH_RIPEMD160" => "lGCZ¡ÓYÄ·Æ¯F4Ÿ\x0C>XX=",
"MHASH_GOST"      => "\x0A%RÎ½õ|­ñQGòU¶C)5»œ,Çâ‹-ž",
"MHASH_TIGER"     => "•:Ãyš\x01¹ýë‘®«— ~g9\\»T0à\x0D",
"MHASH_CRC32"     => "ƒ¸",
"MHASH_CRC32B"    => "¤·Zß"
);

$data = "This is the test of the mhash extension...";

foreach ($supported_hash_al as $hash=>$wanted) {
	$result = mhash(constant($hash), $data);
	if ($result==$wanted) {
		echo "$hash\nok\n";
	} else {
		echo "$hash: ";
		var_dump($wanted);
		echo "$hash: ";
		var_dump($result);
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
