--TEST--
stream_is_local() with nested compression wrappers and allow_url_fopen=0
--EXTENSIONS--
bz2
zlib
--INI--
allow_url_fopen=0
--FILE--
<?php

var_dump(stream_is_local('compress.zlib:///etc/os-release'));
var_dump(stream_is_local('compress.bzip2:///etc/os-release'));
var_dump(stream_is_local('compress.zlib://http://127.0.0.1/example.html'));
var_dump(stream_is_local('compress.bzip2://http://127.0.0.1/example.html'));
var_dump(stream_is_local('compress.zlib://compress.bzip2://http://127.0.0.1/example.html'));

?>
--EXPECT--
bool(true)
bool(true)
bool(false)
bool(false)
bool(false)
