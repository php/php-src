--TEST--
Bug #80092 (ZTS + preload = segfault on shutdown)
--EXTENSIONS--
opcache
--SKIPIF--
<?php
include 'skipif.inc';
if (substr(PHP_OS, 0, 3) == 'WIN') {
    die ("skip not for Windows");
}
?>
--FILE--
<?php

$cmd = [
    PHP_BINARY,
    '-dopcache.enable=1',
    '-dopcache.enable_cli=1',
    '-dopcache.preload=' . __DIR__ . '/preload.inc',
    '-v'
];

$proc = proc_open($cmd, [['null'], ['pipe', 'w'], ['redirect', 1]], $pipes);
echo stream_get_contents($pipes[1]);

?>
--EXPECTF--
preloaded
PHP %s
Copyright (c) The PHP Group
Zend Engine %s
    with Zend OPcache %s
