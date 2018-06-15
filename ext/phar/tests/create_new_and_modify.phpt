--TEST--
Phar: create and modify phar
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--INI--
phar.readonly=0
phar.require_hash=1
--FILE--
<?php

$fname = dirname(__FILE__) . '/' . basename(__FILE__, '.php') . '.phar.php';
$pname = 'phar://' . $fname;

@unlink($fname);

file_put_contents($pname . '/a.php', "brand new!\n");

$phar = new Phar($fname);
$sig1 = $phar->getSignature();

include $pname . '/a.php';

if (function_exists("opcache_get_status")) {
	$status = opcache_get_status();
	if ($status["opcache_enabled"] || (isset($status["file_cache_only"]) && $status["file_cache_only"])) {
		ini_set("opcache.revalidate_freq", "0");
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
===DONE===
--CLEAN--
<?php unlink(dirname(__FILE__) . '/' . basename(__FILE__, '.clean.php') . '.phar.php'); ?>
--EXPECTF--
brand new!
string(40) "%s"
string(40) "%s"
bool(true)
modified!
another!
===DONE===
