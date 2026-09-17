--TEST--
fopen() with bz2 and zlib wrappers shouldn't take exponential time
--EXTENSIONS--
bz2
zlib
--INI--
allow_url_fopen=0
--FILE--
<?php

$target = str_repeat("compress.zlib://compress.bzip2://", 2) . "http://example.com";
var_dump(fopen($target, "r"));

$target = str_repeat("compress.zlib://compress.bzip2://", 10) . "http://example.com";
var_dump(fopen($target, "r"));

$target = str_repeat("compress.zlib://compress.bzip2://", 50) . "http://example.com";
var_dump(fopen($target, "r"));

?>
--EXPECTF--
Warning: fopen(): compress.zlib:// wrapper is disabled in the server configuration by allow_url_fopen=0 in %s on line %d

Warning: fopen(): Failed to open stream: no suitable wrapper could be found in %s on line %d
bool(false)

Warning: fopen(): compress.zlib:// wrapper is disabled in the server configuration by allow_url_fopen=0 in %s on line %d

Warning: fopen(): Failed to open stream: no suitable wrapper could be found in %s on line %d
bool(false)

Warning: fopen(): compress.zlib:// wrapper is disabled in the server configuration by allow_url_fopen=0 in %s on line %d

Warning: fopen(): Failed to open stream: no suitable wrapper could be found in %s on line %d
bool(false)
