--TEST--
Opening deeply nested compression wrappers with allow_url_fopen=0 is not exponential
--EXTENSIONS--
bz2
zlib
--INI--
allow_url_fopen=0
--FILE--
<?php

$target = str_repeat('compress.zlib://compress.bzip2://', 50) . 'http://127.0.0.1/example.html';
$start = hrtime(true);
var_dump(@fopen($target, 'r'));
var_dump(stream_is_local($target));
var_dump((hrtime(true) - $start) / 1e9 < 5);

?>
--EXPECT--
bool(false)
bool(false)
bool(true)
