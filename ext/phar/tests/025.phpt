--TEST--
Phar: phar:// include (repeated names)
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--INI--
phar.require_hash=0
--FILE--
<?php
$fname = __DIR__ . '/' . basename(__FILE__, '.php') . '.phar.php';
$pname = 'phar://' . $fname;
$file = "<?php __HALT_COMPILER(); ?>";

$files = array();
$files['a'] = '<?php echo "This is a\n"; ?>';
$files['b'] = '<?php echo "This is b\n"; ?>';
$files['b/b'] = '<?php echo "This is b/b\n"; ?>';

include 'files/phar_test.inc';

include $pname . '/a';
include $pname . '/b';
include $pname . '/b/b';

?>
===DONE===
--CLEAN--
<?php unlink(__DIR__ . '/' . basename(__FILE__, '.clean.php') . '.phar.php'); ?>
--EXPECT--
This is a
This is b
This is b/b
===DONE===
