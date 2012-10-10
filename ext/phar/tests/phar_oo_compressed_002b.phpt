--TEST--
Phar: context/compress=BZip2
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
<?php if (!extension_loaded("bz2")) die("skip bz2 not present"); ?>
--INI--
phar.readonly=0
phar.require_hash=0
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

$context = stream_context_create(array('phar'=>array('compress'=>Phar::BZ2)));

file_put_contents($pname . '/b', b'new b');
file_put_contents($pname . '/c', b'new c', 0, $context);
file_put_contents($pname . '/d', b'new d');
file_put_contents($pname . '/e', b'new e', 0, $context);

$phar = new Phar($fname);
var_dump(file_get_contents($pname . '/a'));
var_dump($phar['a']->isCompressed());
var_dump(file_get_contents($pname . '/b'));
var_dump($phar['b']->isCompressed());
var_dump(file_get_contents($pname . '/c'));
var_dump($phar['c']->isCompressed());
var_dump(file_get_contents($pname . '/d'));
var_dump($phar['d']->isCompressed());
var_dump(file_get_contents($pname . '/e'));
var_dump($phar['e']->isCompressed());

?>
===DONE===
--CLEAN--
<?php 
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
bool(false)
string(5) "new b"
bool(false)
string(5) "new c"
bool(true)
string(5) "new d"
bool(false)
string(5) "new e"
bool(true)
===DONE===
