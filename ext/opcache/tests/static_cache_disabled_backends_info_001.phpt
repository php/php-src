--TEST--
OPcache static cache disabled backends report unavailable and store APIs return false
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.static_cache.volatile_size_mb=0
opcache.static_cache.pinned_size_mb=0
--FILE--
<?php

function dump_info(string $label, OPcache\StaticCacheInfo $info): void
{
    echo $label, "\n";
    var_dump($info->enabled);
    var_dump($info->available);
    var_dump($info->startup_failed);
    var_dump($info->backend_initialized);
    var_dump($info->configured_memory);
    var_dump($info->failure_reason);
}

dump_info('volatile', OPcache\volatile_cache_info());
dump_info('pinned', OPcache\pinned_cache_info());

var_dump(OPcache\volatile_store('key', 'value'));
var_dump(OPcache\volatile_store_array(['array-key' => 'value']));
var_dump(OPcache\pinned_store('key', 'value'));
var_dump(OPcache\pinned_store_array(['array-key' => 'value']));

?>
--EXPECT--
volatile
bool(false)
bool(false)
bool(false)
bool(false)
int(0)
NULL
pinned
bool(false)
bool(false)
bool(false)
bool(false)
int(0)
NULL
bool(false)
bool(false)
bool(false)
bool(false)
