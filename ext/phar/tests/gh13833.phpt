--TEST--
GH-13836 (Renaming a file in a Phar to an already existing filename causes a NULL pointer dereference)
--EXTENSIONS--
phar
--INI--
phar.require_hash=0
phar.readonly=0
--FILE--
<?php
$fname = __DIR__ . '/gh13836.phar';

$phar = new Phar($fname, 0, 'a.phar');
$phar['x'] = 'hi1';
$phar['y'] = 'hi2';

var_dump(rename("phar://a.phar/x", "phar://a.phar/y"));

var_dump(isset($phar['x']));
var_dump($phar['y']);
?>
--CLEAN--
<?php
unlink(__DIR__ . '/gh13836.phar');
?>
--EXPECTF--
bool(true)
bool(false)
object(PharFileInfo)#2 (2) {
  ["pathName":"SplFileInfo":private]=>
  string(%d) "phar://%sgh13836.phar/y"
  ["fileName":"SplFileInfo":private]=>
  string(1) "y"
}
