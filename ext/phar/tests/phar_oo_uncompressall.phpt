--TEST--
Phar::uncompressAllFiles()
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
<?php if (!extension_loaded("zlib")) die("skip zlib not present"); ?>
--INI--
phar.require_hash=0
phar.readonly=0
--FILE--
<?php
$fname = dirname(__FILE__) . '/' . basename(__FILE__, '.php') . '.phar.php';
$pname = 'phar://' . $fname;
$file = '<?php __HALT_COMPILER(); ?>';

$files = array();
$files['a'] = 'a';
$files['b'] = 'b';
$files['c'] = 'c';

include 'files/phar_test.inc';

$phar = new Phar($fname);

var_dump(file_get_contents($pname . '/a'));
var_dump($phar['a']->isCompressed());
var_dump(file_get_contents($pname . '/b'));
var_dump($phar['b']->isCompressed());
var_dump(file_get_contents($pname . '/c'));
var_dump($phar['c']->isCompressed());

$phar = new Phar($fname);
$phar->compressAllFilesGZ();
var_dump(file_get_contents($pname . '/a'));
var_dump($phar['a']->isCompressedGZ());
var_dump($phar['a']->isCompressedBZIP2());
var_dump(file_get_contents($pname . '/b'));
var_dump($phar['b']->isCompressedGZ());
var_dump($phar['b']->isCompressedBZIP2());
var_dump(file_get_contents($pname . '/c'));
var_dump($phar['c']->isCompressedGZ());
var_dump($phar['b']->isCompressedBZIP2());

$phar->uncompressAllFiles();
var_dump(file_get_contents($pname . '/a'));
var_dump($phar['a']->isCompressed());
var_dump(file_get_contents($pname . '/b'));
var_dump($phar['a']->isCompressed());
var_dump(file_get_contents($pname . '/c'));
var_dump($phar['a']->isCompressed());

?>
===DONE===
--CLEAN--
<?php 
unlink(dirname(__FILE__) . '/' . basename(__FILE__, '.clean.php') . '.phar');
unlink(dirname(__FILE__) . '/' . basename(__FILE__, '.clean.php') . '.phar.php');
?>
--EXPECTF--
string(1) "a"
bool(false)
string(1) "b"
bool(false)
string(1) "c"
bool(false)
string(1) "a"
bool(true)
bool(false)
string(1) "b"
bool(true)
bool(false)
string(1) "c"
bool(true)
bool(false)
string(1) "a"
bool(false)
string(1) "b"
bool(false)
string(1) "c"
bool(false)
===DONE===
