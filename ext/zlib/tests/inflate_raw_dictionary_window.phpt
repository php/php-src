--TEST--
inflate_init(): preset dictionary is honored for raw encoding with a non-default window
--EXTENSIONS--
zlib
--FILE--
<?php
$dict = "the quick brown fox jumps over the lazy dog";
$data = str_repeat($dict . " ", 8);
$opts = ['window' => 10, 'dictionary' => $dict];

$def = deflate_init(ZLIB_ENCODING_RAW, $opts);
$comp = deflate_add($def, $data, ZLIB_FINISH);

$inf = inflate_init(ZLIB_ENCODING_RAW, $opts);
$out = inflate_add($inf, $comp, ZLIB_FINISH);

var_dump($out === $data);
?>
--EXPECT--
bool(true)
