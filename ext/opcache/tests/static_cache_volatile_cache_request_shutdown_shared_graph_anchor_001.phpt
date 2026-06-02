--TEST--
OPcache volatile cache keeps request-held shared graphs anchored during shutdown
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.static_cache.volatile_size_mb=32
--FILE--
<?php

function build_shutdown_payload(string $prefix): array
{
    $rows = [];

    for ($i = 0; $i < 1024; $i++) {
        $rows[] = [
            'id' => $i,
            'text' => str_repeat($prefix, 32),
            'nested' => ['value' => $i * 3],
        ];
    }

    return [
        'name' => 'shutdown-anchor',
        'rows' => $rows,
    ];
}

OPcache\VolatileCache::clear();

var_dump(OPcache\VolatileCache::set('fragment-padding', str_repeat('P', 131072)));
var_dump(OPcache\VolatileCache::set('shutdown-anchor', build_shutdown_payload('A')));
OPcache\VolatileCache::delete('fragment-padding');

$_POST['shutdown-anchor'] = OPcache\VolatileCache::get('shutdown-anchor');

echo $_POST['shutdown-anchor']['name'], "\n";
echo $_POST['shutdown-anchor']['rows'][123]['text'], "\n";
echo $_POST['shutdown-anchor']['rows'][123]['nested']['value'], "\n";

?>
--EXPECT--
bool(true)
bool(true)
shutdown-anchor
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
369
