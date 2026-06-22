--TEST--
OPcache starts with default static cache memory and tracing JIT
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.jit=tracing
opcache.jit_buffer_size=64M
opcache.protect_memory=1
--FILE--
<?php

$config = opcache_get_configuration();
$status = opcache_get_status(false);

var_dump($config['directives']['opcache.static_cache.volatile_size_mb']);
var_dump($config['directives']['opcache.static_cache.stable_size_mb']);
var_dump($status['volatile_cache']->enabled);
var_dump($status['stable_cache']->enabled);
echo "OK\n";

?>
--EXPECT--
int(8388608)
int(8388608)
bool(true)
bool(true)
OK
