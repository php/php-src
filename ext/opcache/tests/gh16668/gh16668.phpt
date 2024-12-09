--TEST--
GH-16668: opcache_compile_file() mistakenly declares top functions
--INI--
opcache.enable=1
opcache.enable_cli=1
--EXTENSIONS--
opcache
--FILE--
<?php
opcache_compile_file(__DIR__ . '/gh16668.inc');
opcache_compile_file(__DIR__ . '/gh16668.inc');
var_dump(function_exists('test'));
require __DIR__ . '/gh16668.inc';
var_dump(function_exists('test'));
test();
?>
--EXPECT--
bool(false)
bool(true)
test()
