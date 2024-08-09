--TEST--
GH-15021: Optimizer only relies on preloaded top-level symbols
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.preload={PWD}/gh15021_preload.inc
--EXTENSIONS--
opcache
--SKIPIF--
<?php
if (PHP_OS_FAMILY == 'Windows') die('skip Preloading is not supported on Windows');
?>
--FILE--
<?php
putenv('RUNTIME=1');
$firstRun = !isset(opcache_get_status()['scripts'][__DIR__ . DIRECTORY_SEPARATOR . 'gh15021_required.inc']);

if ($firstRun) {
    require __DIR__ . '/gh15021_a.inc';
    $expected = 1;
} else {
    require __DIR__ . '/gh15021_b.inc';
    $expected = 2;
}

require __DIR__ . '/gh15021_required.inc';

var_dump(f() === $expected);
?>
--EXPECT--
bool(true)
