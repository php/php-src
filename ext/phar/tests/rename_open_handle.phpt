--TEST--
Phar: renaming an entry with an open file handle
--EXTENSIONS--
phar
--INI--
phar.readonly=0
phar.require_hash=0
--FILE--
<?php
$fname = __DIR__ . '/' . basename(__FILE__, '.php') . '.phar';
$copy = __DIR__ . '/' . basename(__FILE__, '.php') . '.copy.phar';
$pname = 'phar://' . $fname;

$phar = new Phar($fname);
$phar['a'] = 'aaaa';
$phar['b'] = 'bbbb';

$fp = fopen($pname . '/b', 'r');
$fp2 = fopen($pname . '/a', 'r');
var_dump(rename($pname . '/b', $pname . '/c'));
var_dump(fclose($fp));
var_dump(fclose($fp2));

copy($fname, $copy);
$saved = new Phar($copy);
var_dump(isset($saved['b']));
var_dump($saved['a']->getContent());
var_dump($saved['c']->getContent());
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/' . basename(__FILE__, '.clean.php') . '.phar');
@unlink(__DIR__ . '/' . basename(__FILE__, '.clean.php') . '.copy.phar');
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(false)
string(4) "aaaa"
string(4) "bbbb"
