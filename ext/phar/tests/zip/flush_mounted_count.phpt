--TEST--
Phar: writing a zip-based archive with a mounted entry
--EXTENSIONS--
phar
--INI--
phar.readonly=0
phar.require_hash=0
--FILE--
<?php
$fname = __DIR__ . '/' . basename(__FILE__, '.php') . '.zip';
$copy = __DIR__ . '/' . basename(__FILE__, '.php') . '.copy.zip';

$phar = new PharData($fname);
$phar['a'] = 'aaaa';
Phar::mount('phar://' . $fname . '/mounted.txt', __FILE__);
$phar['b'] = 'bbbb';

copy($fname, $copy);
$saved = new PharData($copy);
var_dump(count($saved));
var_dump(isset($saved['mounted.txt']));
var_dump($saved['b']->getContent());
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/' . basename(__FILE__, '.clean.php') . '.zip');
@unlink(__DIR__ . '/' . basename(__FILE__, '.clean.php') . '.copy.zip');
?>
--EXPECT--
int(2)
bool(false)
string(4) "bbbb"
