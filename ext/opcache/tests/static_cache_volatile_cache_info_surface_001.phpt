--TEST--
OPcache volatile cache status and info surface
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.static_cache.volatile_size_mb=32
--FILE--
<?php

$config = opcache_get_configuration();
$status = opcache_get_status();
$info = OPcache\volatile_cache_info();

var_dump($config['directives']['opcache.static_cache.volatile_size_mb']);
var_dump($status['volatile_cache']['enabled']);
var_dump($status['volatile_cache']['available']);
var_dump($status['volatile_cache']['startup_failed']);
var_dump($status['volatile_cache']['backend_initialized']);
var_dump($status['volatile_cache']['configured_memory']);
var_dump($status['volatile_cache']['shared_memory']);
var_dump($status['volatile_cache']['segment_count'] > 0);
var_dump(is_string($status['volatile_cache']['shared_model']));
var_dump($info == $status['volatile_cache']);
var_dump(array_key_exists('failure_reason', $info));

?>
--EXPECTF--
int(33554432)
bool(true)
bool(true)
bool(false)
bool(true)
int(33554432)
int(33554432)
bool(true)
bool(true)
bool(true)
bool(false)
