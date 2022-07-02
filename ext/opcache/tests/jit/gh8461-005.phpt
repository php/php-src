--TEST--
Bug GH-8461 005 (JIT does not account for function re-compile)
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.jit_buffer_size=1M
opcache.jit=1255
opcache.file_update_protection=0
opcache.revalidate_freq=0
opcache.protect_memory=1
--FILE--
<?php

if (!isset(opcache_get_status()['scripts'][__DIR__ . '/gh8461-005.inc'])) {
    $initialRequest = true;
    require __DIR__ . '/gh8461-005.inc';

} else {
    $initialRequest = false;
    $y = 0;
    function test() {
        global $y;
        $y += 1;
    }
}

for ($i = 0; $i < 10; $i++) {
    test();
}

var_dump($initialRequest ? $x : $y);
print "OK";
--EXPECT--
int(10)
OK
