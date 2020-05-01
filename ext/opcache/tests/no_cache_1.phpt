--TEST--
no_cache_1: Opcache indicates if optimizations and caching are enabled
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.no_cache=1
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
  ["no_cache"]=>
  bool(true)
}
