--TEST--
Phar: unsetting an entry with an open file handle zip-based
--EXTENSIONS--
phar
--INI--
phar.readonly=0
phar.require_hash=0
--FILE--
<?php
$fname = __DIR__ . '/' . basename(__FILE__, '.php') . '.zip';

$phar = new PharData($fname);
$phar['a'] = 'aaaa';
$phar['b'] = 'bbbb';

$fp = fopen('phar://' . $fname . '/b', 'r');
unset($phar['b']);
var_dump(fread($fp, 4));
var_dump(fclose($fp));
var_dump(isset($phar['b']));
var_dump($phar['a']->getContent());
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/' . basename(__FILE__, '.clean.php') . '.zip');
?>
--EXPECT--
bool(false)
bool(true)
bool(false)
string(4) "aaaa"
