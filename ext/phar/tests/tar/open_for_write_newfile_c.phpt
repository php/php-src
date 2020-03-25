--TEST--
Phar: fopen a .phar for writing (new file) tar-based
--SKIPIF--
<?php
if (!extension_loaded("phar")) die("skip");
?>
--INI--
phar.readonly=0
phar.require_hash=0
--FILE--
<?php

$fname = __DIR__ . '/' . basename(__FILE__, '.php') . '.phar.tar';
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

var_dump(fopen($alias . '/b/new.php', 'wb'));
include $alias . '/b/c.php';
include $alias . '/b/new.php';

?>

--CLEAN--
<?php unlink(__DIR__ . '/' . basename(__FILE__, '.clean.php') . '.phar.tar'); ?>
--EXPECTF--
Warning: fopen(phar://%sopen_for_write_newfile_c.phar.tar/b/new.php): Failed to open stream: phar error: write operations disabled by the php.ini setting phar.readonly in %sopen_for_write_newfile_c.php on line %d
bool(false)
This is b/c

Warning: include(phar://%sopen_for_write_newfile_c.phar.tar/b/new.php): Failed to open stream: phar error: "b/new.php" is not a file in phar "%sopen_for_write_newfile_c.phar.tar" in %sopen_for_write_newfile_c.php on line %d

Warning: include(): Failed opening 'phar://%sopen_for_write_newfile_c.phar.tar/b/new.php' for inclusion (include_path='%s') in %sopen_for_write_newfile_c.php on line %d

