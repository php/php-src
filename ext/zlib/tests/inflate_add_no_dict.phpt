--TEST--
inflate_add(): Z_NEED_DICT returned when no dictionary provided to inflate_init()
--EXTENSIONS--
zlib
--FILE--
<?php

$dict = "the quick brown fox jumps over the lazy dog";
$data = "the quick brown fox";

$dc = deflate_init(ZLIB_ENCODING_DEFLATE, ['dictionary' => $dict]);
$compressed = deflate_add($dc, $data, ZLIB_FINISH);

// Inflate without supplying the dictionary
$ic = inflate_init(ZLIB_ENCODING_DEFLATE);
$result = inflate_add($ic, $compressed, ZLIB_SYNC_FLUSH);
var_dump($result);

?>
--EXPECTF--
Warning: inflate_add(): Inflating this data requires a preset dictionary, please specify it in the options array of inflate_init() in %s on line %d
bool(false)
