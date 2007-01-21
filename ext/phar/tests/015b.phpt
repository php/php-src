--TEST--
Phar::mapPhar valid file (bzip2)
--SKIPIF--
<?php if (!extension_loaded("phar")) print "skip";?>
<?php if (!extension_loaded("bz2")) print "skip bz2 not present"; ?>
--INI--
phar.require_hash=0
--FILE--
<?php
$fname = dirname(__FILE__) . '/' . basename(__FILE__, '.php') . '.phar.php';
$pname = 'phar://' . $fname;
$file = "<?php __HALT_COMPILER(); ?>";

$files = array();
$files['a'] = array('cont'=>'Hello World', 'comp'=>pack('H*', '425a6839314159265359d872012f00000157800010400000400080060490002000220686d420c988c769e8281f8bb9229c28486c39009780'),'flags'=>0x00002000);
include 'phar_test.inc';

var_dump(file_get_contents($pname . '/a'));
?>
--CLEAN--
<?php unlink(dirname(__FILE__) . '/' . basename(__FILE__, '.clean.php') . '.phar.php'); ?>
--EXPECT--
string(11) "Hello World"
