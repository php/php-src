--TEST--
001: O+ works in CLI
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_cache_only=0
--EXTENSIONS--
opcache
--FILE--
<?php
$config = opcache_get_configuration();
$status = opcache_get_status();
var_dump($config["directives"]["opcache.enable"]);
var_dump($config["directives"]["opcache.enable_cli"]);
var_dump($status["opcache_enabled"]);
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
