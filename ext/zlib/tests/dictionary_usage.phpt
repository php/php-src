--TEST--
Test dictionary usage on zlib methods
--FILE--
<?php

$dict = range("a", "z");

$r = deflate_init(ZLIB_ENCODING_DEFLATE, ["dictionary" => $dict]);
$a = deflate_add($r, "abdcde", ZLIB_FINISH);
var_dump($a);

$r = inflate_init(ZLIB_ENCODING_DEFLATE, ["dictionary" => $dict]);
var_dump(inflate_add($r, $a, ZLIB_FINISH));


$r = inflate_init(ZLIB_ENCODING_DEFLATE, ["dictionary" => ["8"] + range("a", "z")]);
var_dump(inflate_add($r, $a, ZLIB_FINISH));

?>
--EXPECTF--
string(%d) "%s"
string(6) "abdcde"

Warning: inflate_add(): dictionary does match expected dictionary (incorrect adler32 hash) in %s on line %d
bool(false)
