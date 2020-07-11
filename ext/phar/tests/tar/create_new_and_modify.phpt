--TEST--
Phar: create and modify tar-based phar
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--INI--
phar.readonly=0
opcache.validate_timestamps=1
--FILE--
<?php

$fname = __DIR__ . '/' . basename(__FILE__, '.php') . '.phar.tar.php';
$pname = 'phar://' . $fname;

@unlink($fname);

file_put_contents($pname . '/a.php', "brand new!\n");

if (function_exists("opcache_get_status")) {
    $status = opcache_get_status();
    if (is_array($status) && ($status["opcache_enabled"] || (isset($status["file_cache_only"]) && $status["file_cache_only"]))) {
        opcache_invalidate($pname . '/a.php', true);
        // opcache_invalidate is buggy and doesn't work as expected so we add a
        // minor delay here.
        sleep(2);
    }
}

$phar = new Phar($fname);
var_dump($phar->isFileFormat(Phar::TAR));
$sig1 = md5_file($fname);

include $pname . '/a.php';

file_put_contents($pname .'/a.php', "modified!\n");
file_put_contents($pname .'/b.php', "another!\n");

$phar = new Phar($fname);
$sig2 = md5_file($fname);

var_dump($sig1 != $sig2);

include $pname . '/a.php';
include $pname . '/b.php';

?>
--CLEAN--
<?php unlink(__DIR__ . '/' . basename(__FILE__, '.clean.php') . '.phar.tar.php'); ?>
--EXPECT--
bool(true)
brand new!
bool(true)
modified!
another!
