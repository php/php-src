--TEST--
Test dictionary usage on zlib methods
--SKIPIF--
<?php if(!extension_loaded('zlib')) die('skip zlib extension not loaded'); ?>
--FILE--
<?php

$dict = range("a", "z");

$r = deflate_init(ZLIB_ENCODING_DEFLATE, ["dictionary" => $dict]);
$a = deflate_add($r, "abdcde", ZLIB_FINISH);
var_dump($a);

$r = deflate_init(ZLIB_ENCODING_DEFLATE, ["dictionary" => implode("\0", $dict)."\0"]);
$dictStr_a = deflate_add($r, "abdcde", ZLIB_FINISH);
var_dump($dictStr_a === $a);

$r = inflate_init(ZLIB_ENCODING_DEFLATE, ["dictionary" => $dict]);
var_dump(inflate_add($r, $a, ZLIB_FINISH));

$r = inflate_init(ZLIB_ENCODING_DEFLATE, ["dictionary" => ["8"] + range("a", "z")]);
var_dump(inflate_add($r, $a, ZLIB_FINISH));

?>
--EXPECTF--
string(%d) "%s"
bool(true)
string(6) "abdcde"

Warning: inflate_add(): Dictionary does not match expected dictionary (incorrect adler32 hash) in %s on line %d
bool(false)
