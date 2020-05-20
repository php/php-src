--TEST--
allow_cache_1: Opcache indicates if optimizations and caching are enabled
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.allow_cache=0
opcache.preload=
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
$status = opcache_get_status();
var_dump($status);
?>
--EXPECT--
array(3) {
  ["optimizations_enabled"]=>
  bool(true)
  ["opcache_enabled"]=>
  bool(false)
  ["allow_cache"]=>
  bool(false)
}
