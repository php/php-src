--TEST--
Phar: fopen a .phar for writing (existing file) tar-based
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
<?php if (version_compare(PHP_VERSION, "5.3", ">")) die("skip requires 5.2 or earlier"); ?>
--INI--
phar.readonly=0
phar.require_hash=0
--FILE--
<?php

$fname = dirname(__FILE__) . '/' . basename(__FILE__, '.php') . '.phar.tar';
$alias = 'phar://' . $fname;

$phar = new Phar($fname);
$phar->setStub("<?php __HALT_COMPILER(); ?>");

$files = array();

$files['a.php'] = '<?php echo "This is a\n"; ?>';
$files['b.php'] = '<?php echo "This is b\n"; ?>';
$files['b/c.php'] = '<?php echo "This is b/c\n"; ?>';

foreach ($files as $n => $file) {
	$phar[$n] = $file;
}

$phar->stopBuffering();
ini_set('phar.readonly', 1);

$fp = fopen($alias . '/b/c.php', 'wb');
fwrite($fp, b'extra');
fclose($fp);

include $alias . '/b/c.php';

?>

===DONE===
--CLEAN--
<?php unlink(dirname(__FILE__) . '/' . basename(__FILE__, '.clean.php') . '.phar.tar'); ?>
--EXPECTF--

Warning: fopen(phar://%sopen_for_write_existing_c_5_2.phar.tar/b/c.php): failed to open stream: phar error: write operations disabled by the php.ini setting phar.readonly in %sopen_for_write_existing_c_5_2.php on line %d

Warning: fwrite(): supplied argument is not a valid stream resource in %sopen_for_write_existing_c_5_2.php on line %d

Warning: fclose(): supplied argument is not a valid stream resource in %sopen_for_write_existing_c_5_2.php on line %d
This is b/c

===DONE===
