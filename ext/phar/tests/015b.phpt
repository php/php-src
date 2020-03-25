--TEST--
Phar::mapPhar valid file (bzip2)
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip");?>
<?php if (!extension_loaded("bz2")) die("skip bz2 not present"); ?>
--INI--
phar.require_hash=0
--FILE--
<?php
$fname = __DIR__ . '/' . basename(__FILE__, '.php') . '.phar.php';
$pname = 'phar://' . $fname;
$file = "<?php __HALT_COMPILER(); ?>";

$files = array();
$files['a'] = array('cont'=>'Hello World', 'comp'=>pack('H*', '425a6834314159265359065c89da0000009780400000400080060490002000310c082031a916c41d41e2ee48a70a1200cb913b40'),'flags'=>0x00002000);
include 'files/phar_test.inc';

var_dump(file_get_contents($pname . '/a'));
?>
--CLEAN--
<?php unlink(__DIR__ . '/' . basename(__FILE__, '.clean.php') . '.phar.php'); ?>
--EXPECT--
string(11) "Hello World"
