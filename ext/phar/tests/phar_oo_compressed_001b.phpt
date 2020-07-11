--TEST--
Phar: PharFileInfo::compress(Phar::BZ2)
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
<?php if (!extension_loaded("bz2")) die("skip bz2 not present"); ?>
--INI--
phar.require_hash=0
phar.readonly=0
--FILE--
<?php
$fname = __DIR__ . '/' . basename(__FILE__, '.php') . '.phar.php';
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

$phar['a'] = 'new a';
$phar['a']->decompress();
$phar['b'] = 'new b';
$phar['b']->compress(Phar::BZ2);
$phar['d'] = 'new d';

$phar = new Phar($fname);
var_dump(file_get_contents($pname . '/a'));
var_dump($phar['a']->isCompressed());
var_dump(file_get_contents($pname . '/b'));
var_dump($phar['b']->isCompressed());
var_dump(file_get_contents($pname . '/c'));
var_dump($phar['c']->isCompressed());
var_dump(file_get_contents($pname . '/d'));
var_dump($phar['d']->isCompressed());

?>
--CLEAN--
<?php
unlink(__DIR__ . '/' . basename(__FILE__, '.clean.php') . '.phar.php');
?>
--EXPECT--
string(1) "a"
bool(false)
string(1) "b"
bool(false)
string(1) "c"
bool(false)
string(5) "new a"
bool(false)
string(5) "new b"
bool(true)
string(1) "c"
bool(false)
string(5) "new d"
bool(false)
