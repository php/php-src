--TEST--
Bug #80092 (ZTS + preload = segfault on shutdown)
--SKIPIF--
<?php
include 'skipif.inc';
if (substr(PHP_OS, 0, 3) == 'WIN') {
    die ("skip not for Windows");
}
$extDir = ini_get('extension_dir');
if (!file_exists($extDir . '/opcache.so')) {
    die ('skip opcache shared object not found in extension_dir');
}
?>
--FILE--
<?php

$cmd = [
    PHP_BINARY,
    '-dextension_dir=' . ini_get('extension_dir'),
    '-dzend_extension=opcache.so',
    '-dopcache.enable=1',
    '-dopcache.enable_cli=1',
    '-dopcache.preload=' . __DIR__ . '/preload.inc',
    '-v'
];

$proc = proc_open($cmd, [['null'], ['pipe', 'w'], ['redirect', 1]], $pipes);
$output = stream_get_contents($pipes[1]);

// Ignore warnings if opcache loads twice or duplicate modules are loaded by searching for the preloaded marker
$flag = false;
foreach (explode("\n", $output) as $line) {
	if ($line === "preloaded") {
		$flag = true;
	}
	if ($flag) {
		echo $line, "\n";
	}
}

?>
--EXPECTF--
preloaded
PHP %s
Copyright (c) The PHP Group
Zend Engine %s
    with Zend OPcache %s
