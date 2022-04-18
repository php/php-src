--TEST--
Phar: create and modify phar
--EXTENSIONS--
phar
--INI--
phar.readonly=0
phar.require_hash=1
opcache.validate_timestamps=1
--FILE--
<?php

$fname = __DIR__ . '/' . basename(__FILE__, '.php') . '.phar.php';
$pname = 'phar://' . $fname;

@unlink($fname);

file_put_contents($pname . '/a.php', "brand new!\n");

$phar = new Phar($fname);
$sig1 = $phar->getSignature();

include $pname . '/a.php';

if (function_exists("opcache_get_status")) {
    $status = opcache_get_status();
    if (is_array($status) && ($status["opcache_enabled"] || (isset($status["file_cache_only"]) && $status["file_cache_only"]))) {
        opcache_invalidate($pname . '/a.php', true);
        // opcache_invalidate is buggy and doesn't work as expected so we add a
        // minor delay here.
        sleep(2);
    }
}

file_put_contents($pname .'/a.php', "modified!\n");
file_put_contents($pname .'/b.php', "another!\n");

$phar = new Phar($fname);
$sig2 = $phar->getSignature();

var_dump($sig1['hash']);
var_dump($sig2['hash']);
var_dump($sig1['hash'] != $sig2['hash']);

include $pname . '/a.php';
include $pname . '/b.php';

?>
--CLEAN--
<?php unlink(__DIR__ . '/' . basename(__FILE__, '.clean.php') . '.phar.php'); ?>
--EXPECTF--
brand new!
string(%d) "%s"
string(%d) "%s"
bool(true)
modified!
another!
