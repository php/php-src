--TEST--
Phar: fopen a .phar for writing (new file)
--SKIPIF--
<?php
if (!extension_loaded("phar")) die("skip");
?>
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

var_dump(fopen($pname . '/b/new.php', 'wb'));
include $pname . '/b/c.php';
include $pname . '/b/new.php';
?>

--CLEAN--
<?php unlink(__DIR__ . '/' . basename(__FILE__, '.clean.php') . '.phar.php'); ?>
--EXPECTF--
Warning: fopen(phar://%sopen_for_write_newfile_c.phar.php/b/new.php): Failed to open stream: phar error: write operations disabled by the php.ini setting phar.readonly in %sopen_for_write_newfile_c.php on line %d
bool(false)
This is b/c

Warning: include(phar://%sopen_for_write_newfile_c.phar.php/b/new.php): Failed to open stream: phar error: "b/new.php" is not a file in phar "%sopen_for_write_newfile_c.phar.php" in %sopen_for_write_newfile_c.php on line %d

Warning: include(): Failed opening 'phar://%sopen_for_write_newfile_c.phar.php/b/new.php' for inclusion (include_path='%s') in %sopen_for_write_newfile_c.php on line %d

