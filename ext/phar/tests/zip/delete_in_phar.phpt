--TEST--
Phar: delete a file within a zip-based .phar
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--INI--
phar.readonly=0
phar.require_hash=0
--FILE--
<?php
include dirname(__FILE__) . '/tarmaker.php.inc';
$fname = dirname(__FILE__) . '/' . basename(__FILE__, '.php') . '.phar.php';
$pname = 'phar://' . $fname;

$a = new tarmaker($fname, 'none');
$a->init();
$a->addFile('a.php', '<?php echo "This is a\n"; ?>');
$a->addFile('b.php', '<?php echo "This is b\n"; ?>');
$a->addFile('b/c.php', '<?php echo "This is b/c\n"; ?>');
$a->addFile('.phar/stub.php', '<?php __HALT_COMPILER(); ?>');
$a->close();

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
<?php unlink(dirname(__FILE__) . '/' . basename(__FILE__, '.clean.php') . '.phar.php'); ?>
--EXPECTF--
This is a
This is b
This is b/c
===AFTER===
This is a
This is b

Warning: include(%sdelete_in_phar.phar.php/b/c.php): failed to open stream: phar error: "b/c.php" is not a file in phar "%sdelete_in_phar.phar.php" in %sdelete_in_phar.php on line %d

Warning: include(): Failed opening 'phar://%sdelete_in_phar.phar.php/b/c.php' for inclusion (include_path='%s') in %sdelete_in_phar.php on line %d

===DONE===
		