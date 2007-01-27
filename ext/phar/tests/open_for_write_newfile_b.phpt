--TEST--
Phar: fopen a .phar for writing (new file)
--SKIPIF--
<?php if (!extension_loaded("phar")) print "skip"; ?>
--INI--
phar.readonly=1
phar.require_hash=0
--FILE--
<?php
$fname = dirname(__FILE__) . '/' . basename(__FILE__, '.php') . '.phar.php';
$pname = 'phar://' . $fname;
$file = "<?php __HALT_COMPILER(); ?>";

$files = array();
$files['a.php'] = '<?php echo "This is a\n"; ?>';
$files['b.php'] = '<?php echo "This is b\n"; ?>';
$files['b/c.php'] = '<?php echo "This is b/c\n"; ?>';
include 'phar_test.inc';

function err_handler($errno, $errstr, $errfile, $errline) {
  echo "Catchable fatal error: $errstr in $errfile on line $errline\n";
}

set_error_handler("err_handler", E_RECOVERABLE_ERROR);

$fp = fopen($pname . '/b/new.php', 'wb');
fwrite($fp, 'extra');
fclose($fp);
include $pname . '/b/c.php';
include $pname . '/b/new.php';
?>

===DONE===
--CLEAN--
<?php unlink(dirname(__FILE__) . '/' . basename(__FILE__, '.clean.php') . '.phar.php'); ?>
--EXPECTF--

Catchable fatal error: fopen(): phar error: file "%sopen_for_write_newfile_b.phar.php" cannot opened for writing, disabled by ini setting in %sopen_for_write_newfile_b.php on line %d

Warning: fopen(phar://%sopen_for_write_newfile_b.phar.php/b/new.php): failed to open stream: phar error: file "b/new.php" could not be created in phar "%sopen_for_write_newfile_b.phar.php" in %sopen_for_write_newfile_b.php on line %d

Warning: fwrite()%sresource%sin %sopen_for_write_newfile_b.php on line %d

Warning: fclose(): supplied argument is not a valid stream resource in %sopen_for_write_newfile_b.php on line %d
This is b/c

Warning: include(phar://%sopen_for_write_newfile_b.phar.php/b/new.php): failed to open stream: phar error: "b/new.php" is not a file in phar "%sopen_for_write_newfile_b.phar.php" in %sopen_for_write_newfile_b.php on line %d

Warning: include(): Failed opening 'phar://%sopen_for_write_newfile_b.phar.php/b/new.php' for inclusion (include_path='%s') in %sopen_for_write_newfile_b.php on line %d

===DONE===
