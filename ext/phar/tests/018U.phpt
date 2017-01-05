--TEST--
Phar: opendir test, root directory
--SKIPIF--
<?php
if (!extension_loaded("phar")) die("skip");
if (version_compare(PHP_VERSION, "6.0", "!=")) die("skip Unicode support required");
?>
--INI--
phar.require_hash=0
--FILE--
<?php
$fname = dirname(__FILE__) . '/' . basename(__FILE__, '.php') . '.phar.php';
$pname = 'phar://' . $fname;
$file = b"<?php
Phar::mapPhar('hio');
__HALT_COMPILER(); ?>";

$files = array();
$files['a'] = 'a';
$files['b/a'] = 'b';
include 'files/phar_test.inc';

include $fname;
$dir = opendir('phar://hio/');
while (false !== ($a = readdir($dir))) {
	var_dump($a);
	var_dump(is_dir('phar://hio/' . $a));
}
?>
--CLEAN--
<?php unlink(dirname(__FILE__) . '/' . basename(__FILE__, '.clean.php') . '.phar.php'); ?>
--EXPECT--
unicode(1) "a"
bool(false)
unicode(1) "b"
bool(true)
