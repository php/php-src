--TEST--
Phar: unsetting an entry with an open file handle tar-based
--EXTENSIONS--
phar
--INI--
phar.readonly=0
phar.require_hash=0
--FILE--
<?php
$fname = __DIR__ . '/' . basename(__FILE__, '.php') . '.tar';
$copy = __DIR__ . '/' . basename(__FILE__, '.php') . '.copy.tar';

$phar = new PharData($fname);
$phar['a'] = 'aaaa';
$phar['b'] = 'bbbb';

$fp = fopen('phar://' . $fname . '/b', 'r');
$fp2 = fopen('phar://' . $fname . '/b', 'r');
unset($phar['b']);
var_dump(fread($fp, 4));
var_dump(fclose($fp));
var_dump(fclose($fp2));
var_dump(isset($phar['b']));
var_dump($phar['a']->getContent());

copy($fname, $copy);
$saved = new PharData($copy);
var_dump(count($saved));
var_dump($saved['a']->getContent());
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/' . basename(__FILE__, '.clean.php') . '.tar');
@unlink(__DIR__ . '/' . basename(__FILE__, '.clean.php') . '.copy.tar');
?>
--EXPECT--
bool(false)
bool(true)
bool(true)
bool(false)
string(4) "aaaa"
int(1)
string(4) "aaaa"
