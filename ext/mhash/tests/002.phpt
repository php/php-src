--TEST--
mhash_get_block_size() & mhash_get_hash_name() test
--SKIPIF--
<?php
	include "skip.inc";
?>
--FILE--
<?php
	$hc = mhash_count() + 1;
	
	for ($i=0; $i<$hc; $i++) {
		if (($hn = mhash_get_hash_name($i))) {
			echo $hn . "\t->\t" . mhash_get_block_size($i) . "\n";
		}
	}
?>
--EXPECT--
CRC32	->	4
MD5	->	16
SHA1	->	20
HAVAL256	->	32
RIPEMD160	->	20
TIGER	->	24
GOST	->	32
CRC32B	->	4
HAVAL224	->	28
HAVAL192	->	24
HAVAL160	->	20
