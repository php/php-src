--TEST--
Phar: PHP bug #47085: "rename() returns true even if the file in PHAR does not exist"
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--INI--
phar.require_hash=0
phar.readonly=0
--FILE--
<?php
$fname = __DIR__ . '/' . basename(__FILE__, '.php') . '.phar';

$phar = new Phar($fname, 0, 'a.phar');
$phar['x'] = 'hi';
unset($phar);
rename("phar://a.phar/x", "phar://a.phar/y");
var_dump(rename("phar://a.phar/x", "phar://a.phar/y"));
?>
--CLEAN--
<?php unlink(__DIR__ . '/' . basename(__FILE__, '.clean.php') . '.phar');?>
--EXPECTF--
Warning: rename(): phar error: cannot rename "phar://a.phar/x" to "phar://a.phar/y" from extracted phar archive, source does not exist in %sbug47085.php on line %d
bool(false)
