--TEST--
Phar: writing to an archive with open file handles on a compressed entry
--EXTENSIONS--
phar
zlib
--INI--
phar.readonly=0
phar.require_hash=0
--FILE--
<?php
$fname = __DIR__ . '/' . basename(__FILE__, '.php') . '.phar';
$copy = __DIR__ . '/' . basename(__FILE__, '.php') . '.copy.phar';

$phar = new Phar($fname);
$phar['a'] = 'aaaa';
$phar['b'] = 'bbbb';
$phar['a']->compress(Phar::GZ);
$phar['b']->compress(Phar::GZ);

$fp = fopen('phar://' . $fname . '/b', 'r');
$fp2 = fopen('phar://' . $fname . '/b', 'r');
var_dump(fread($fp, 4));
$phar['c'] = 'cccc';
unset($phar['b']);
$phar['d'] = 'dddd';
var_dump(fclose($fp));
var_dump(fclose($fp2));

copy($fname, $copy);
$saved = new Phar($copy);
var_dump(count($saved));
var_dump($saved['a']->getContent());
var_dump($saved['d']->getContent());
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/' . basename(__FILE__, '.clean.php') . '.phar');
@unlink(__DIR__ . '/' . basename(__FILE__, '.clean.php') . '.copy.phar');
?>
--EXPECT--
string(4) "bbbb"
bool(true)
bool(true)
int(3)
string(4) "aaaa"
string(4) "dddd"
