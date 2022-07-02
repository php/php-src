--TEST--
Phar: fopen a .phar for writing (existing file)
--EXTENSIONS--
phar
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

var_dump(fopen($pname . '/b/c.php', 'wb'));
include $pname . '/b/c.php';
?>
--CLEAN--
<?php unlink(__DIR__ . '/' . basename(__FILE__, '.clean.php') . '.phar.php'); ?>
--EXPECTF--
Warning: fopen(phar://%sopen_for_write_existing_c.phar.php/b/c.php): Failed to open stream: phar error: write operations disabled by the php.ini setting phar.readonly in %sopen_for_write_existing_c.php on line %d
bool(false)
This is b/c
