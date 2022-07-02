--TEST--
Phar: delete a file within a .phar
--EXTENSIONS--
phar
--INI--
phar.readonly=0
phar.require_hash=0
opcache.validate_timestamps=1
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

--CLEAN--
<?php unlink(__DIR__ . '/' . basename(__FILE__, '.clean.php') . '.phar.php'); ?>
--EXPECTF--
This is a
This is b
This is b/c
===AFTER===
This is a
This is b

Warning: include(%sdelete_in_phar.phar.php/b/c.php): Failed to open stream: phar error: "b/c.php" is not a file in phar "%sdelete_in_phar.phar.php" in %sdelete_in_phar.php on line %d

Warning: include(): Failed opening 'phar://%sdelete_in_phar.phar.php/b/c.php' for inclusion (include_path='%s') in %sdelete_in_phar.php on line %d

