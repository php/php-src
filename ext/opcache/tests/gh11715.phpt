--TEST--
GH-11715 (opcache.interned_strings_buffer either has no effect or opcache_get_status() / phpinfo() is wrong)
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.interned_strings_buffer=16
--FILE--
<?php

$info = opcache_get_status()['interned_strings_usage'];
var_dump($info['used_memory'] + $info['free_memory']);
var_dump($info['buffer_size']);

?>
--EXPECT--
int(16777216)
int(16777216)
