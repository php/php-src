--TEST--
Phar: delete a file within a tar-based .phar (confirm disk file is changed)
--EXTENSIONS--
phar
--INI--
phar.readonly=0
phar.require_hash=0
opcache.validate_timestamps=1
--FILE--
<?php

$fname = __DIR__ . '/' . basename(__FILE__, '.php') . '.phar.tar';
$alias = 'phar://' . $fname;

$phar = new Phar($fname);
$phar['a.php'] = '<?php echo "This is a\n"; ?>';
$phar['b.php'] = '<?php echo "This is b\n"; ?>';
$phar['b/c.php'] = '<?php echo "This is b/c\n"; ?>';
$phar->setStub('<?php __HALT_COMPILER(); ?>');
$phar->stopBuffering();

include $alias . '/a.php';
include $alias . '/b.php';
include $alias . '/b/c.php';
$md5 = md5_file($fname);
unlink($alias . '/b/c.php');
clearstatcache();
$md52 = md5_file($fname);
if ($md5 == $md52) echo 'file was not modified';
?>
===AFTER===
<?php
include 'phar://' . __DIR__ . '/' . basename(__FILE__, '.php') . '.phar.tar/a.php';
include 'phar://' . __DIR__ . '/' . basename(__FILE__, '.php') . '.phar.tar/b.php';
include 'phar://' . __DIR__ . '/' . basename(__FILE__, '.php') . '.phar.tar/b/c.php';
?>

--CLEAN--
<?php unlink(__DIR__ . '/' . basename(__FILE__, '.clean.php') . '.phar.tar'); ?>
--EXPECTF--
This is a
This is b
This is b/c
===AFTER===
This is a
This is b

Warning: include(%sdelete_in_phar_confirm.phar.tar/b/c.php): Failed to open stream: phar error: "b/c.php" is not a file in phar "%sdelete_in_phar_confirm.phar.tar" in %sdelete_in_phar_confirm.php on line %d

Warning: include(): Failed opening 'phar://%sdelete_in_phar_confirm.phar.tar/b/c.php' for inclusion (include_path='%s') in %sdelete_in_phar_confirm.php on line %d

