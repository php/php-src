--TEST--
mhash_get_block_size() & mhash_get_hash_name() test
--SKIPIF--
<?php
	include "skip_mhash.inc";
?>
--FILE--
<?php
$supported_hash_al = array(
"MD5"       => 16,
"MD4"       => 16,
"SHA1"      => 20,
"SHA256"    => 32,
"HAVAL256"  => 32,
"HAVAL192"  => 24,
"HAVAL224"  => 28,
"HAVAL160"  => 20,
"HAVAL128"  => 16,
"RIPEMD160" => 20,
"GOST"      => 32,
"TIGER"     => 24,
"TIGER160"  => 20,
"TIGER128"  => 16,
"CRC32"     => 4,
"CRC32B"    => 4,
"ADLER32"   => 4,
"NA_XYZ"    => 0   /* verify that the algorithm works */
);

$hc = mhash_count() + 1;

$known_hash_al = array();	
for ($i=0; $i < $hc; $i++) {
	$known_hash_al[mhash_get_hash_name($i)] = $i;
}

foreach ($supported_hash_al as $name => $len) {
	if (array_key_exists($name, $known_hash_al)) {
		$len = mhash_get_block_size($known_hash_al[$name]);
		echo "$name = $len\n";
	} else {
		echo "$name ? $len\n";
	} 
}
?>
--EXPECTREGEX--
MD5 . 16
MD4 . 16
SHA1 . 20
SHA256 . 32
HAVAL256 . 32
HAVAL192 . 24
HAVAL224 . 28
HAVAL160 . 20
HAVAL128 . 16
RIPEMD160 . 20
GOST . 32
TIGER . 24
TIGER160 . 20
TIGER128 . 16
CRC32 . 4
CRC32B . 4
ADLER32 . 4
NA_XYZ . 0
