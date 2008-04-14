--TEST--
Phar: fopen/stat edge cases
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--INI--
phar.readonly=0
phar.require_hash=0
--FILE--
<?php
$fname = dirname(__FILE__) . '/' . basename(__FILE__, '.php') . '.phar.php';
$fname2 = dirname(__FILE__) . '/' . basename(__FILE__, '.php') . '.2.phar.php';
$pname = 'phar://' . $fname;
$pname2 = 'phar://' . $fname2;

// append
$a = fopen($pname . '/b/c.php', 'a');
// invalid pharname
$a = fopen($pname . '.phar.gz', 'r');
// test phar_open_url() with quiet stat for code coverage
var_dump(file_exists($pname . '.phar.gz/hi'));
// test open for write with new phar
$a = fopen($pname . '/hi', 'w');
fclose($a);
// test open for write with corrupted phar
file_put_contents($fname2, '<?php oh crap __HALT_COMPILER();');
$a = fopen($pname2 . '/hi', 'w');
$a = fopen('phar://', 'r');
$a = fopen('phar://foo.phar', 'r');
?>

===DONE===
--CLEAN--
<?php unlink(dirname(__FILE__) . '/' . basename(__FILE__, '.clean.php') . '.phar.php'); ?>
<?php unlink(dirname(__FILE__) . '/' . basename(__FILE__, '.clean.php') . '.2.phar.php'); ?>
--EXPECTF--
Warning: fopen(phar://%sfopen_edgecases.phar.php/b/c.php): failed to open stream: phar error: open mode append not supported in %sfopen_edgecases.php on line %d

Warning: fopen(phar://%sfopen_edgecases.phar.php.phar.gz): failed to open stream: phar error: invalid url "phar://%sfopen_edgecases.phar.php.phar.gz" (cannot contain .phar.php and .phar.gz/.phar.bz2) in %sfopen_edgecases.php on line %d
bool(false)

Warning: fopen(phar://%sfopen_edgecases.2.phar.php/hi): failed to open stream: internal corruption of phar "%sfopen_edgecases.2.phar.php" (truncated manifest at stub end) in %sfopen_edgecases.php on line %d

Warning: fopen(phar://): failed to open stream: phar error: no directory in "phar://", must have at least phar:/// for root directory (always use full path to a new phar) in %sfopen_edgecases.php on line %d

Warning: fopen(phar://foo.phar): failed to open stream: operation failed in %sfopen_edgecases.php on line %d

===DONE===