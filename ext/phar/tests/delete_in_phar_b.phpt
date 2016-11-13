--TEST--
Phar: delete a file within a .phar
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--INI--
phar.readonly=1
phar.require_hash=0
--FILE--
<?php
$fname = __DIR__ . '/' . basename(__FILE__, '.php') . '.phar.php';
$pname = 'phar://' . $fname;
$file = "<?php __HALT_COMPILER(); ?>";

$files = array();
$files['a.php'] = '<?php echo "This is a\n"; ?>';
$files['b.php'] = '<?php echo "This is b\n"; ?>';
$files['b/c.php'] = '<?php echo "This is b/c\n"; ?>';
include 'files/phar_test.inc';

include $pname . '/a.php';
include $pname . '/b.php';
include $pname . '/b/c.php';
unlink($pname . '/b/c.php');
?>
===AFTER===
<?php
include $pname . '/a.php';
include $pname . '/b.php';
include $pname . '/b/c.php';
?>

===DONE===
--CLEAN--
<?php unlink(__DIR__ . '/' . basename(__FILE__, '.clean.php') . '.phar.php'); ?>
--EXPECTF--
This is a
This is b
This is b/c

Warning: unlink(): phar error: write operations disabled by the php.ini setting phar.readonly in %sdelete_in_phar_b.php on line %d
===AFTER===
This is a
This is b
This is b/c

===DONE===
