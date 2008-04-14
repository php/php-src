--TEST--
Phar: fopen/stat/fseek/unlink/rename edge cases
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--INI--
phar.readonly=0
phar.require_hash=0
--FILE--
<?php
$fname = dirname(__FILE__) . '/' . basename(__FILE__, '.php') . '.phar.php';
$fname2 = dirname(__FILE__) . '/' . basename(__FILE__, '.php') . '.2.phar.php';
$fname3 = dirname(__FILE__) . '/' . basename(__FILE__, '.php') . '.3.phar.php';
$pname = 'phar://' . $fname;
$pname2 = 'phar://' . $fname2;
$pname3 = 'phar://' . $fname3;

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

ini_set('phar.extract_list', 'test.phar=' . dirname(__FILE__) . '/ump');
$a = fopen('phar://test.phar/oops', 'r');

file_put_contents($pname . '/hi', 'hi');
$a = fopen($pname . '/hi', 'r');
var_dump(fseek($a, 1), ftell($a));
var_dump(fseek($a, 1, SEEK_CUR), ftell($a));
fclose($a);

var_dump(stat('phar://'));
var_dump(stat('phar://foo.phar'));
var_dump(is_dir($pname));
ini_set('phar.extract_list', 'test.phar=' . dirname(__FILE__));
var_dump(file_exists('phar://test.phar/' . basename(__FILE__)));
var_dump(file_exists('phar://test.phar/@#$^&*%$#'));

// this tests coverage of the case where the phar exists and has no files
$phar = new Phar($fname3);
var_dump(file_exists($pname3 . '/test'));

unlink($pname2 . '/hi');
unlink('phar://');
unlink('phar://foo.phar');
unlink('phar://test.phar/' . basename(__FILE__));
unlink($pname . '/oops');

rename('phar://', 'phar://');
rename($pname . '/hi', 'phar://');
rename('phar://foo.phar/hi', 'phar://');
rename($pname . '/hi', 'phar://foo.phar/hi');

ini_set('phar.readonly', 1);
rename($pname . '/hi', $pname . '/there');
ini_set('phar.readonly', 0);

rename('phar://test.phar/' . basename(__FILE__), 'phar://test.phar/hi');
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

Warning: fopen(phar://test.phar/oops): failed to open stream: phar error: file "oops" extracted from "test.phar" could not be opened in %sfopen_edgecases.php on line %d
int(0)
int(1)
int(0)
int(2)

Warning: stat(): stat failed for phar:// in %sfopen_edgecases.php on line %d
bool(false)

Warning: stat(): stat failed for phar://foo.phar in %sfopen_edgecases.php on line %d
bool(false)
bool(true)
bool(true)
bool(false)
bool(false)

Warning: unlink(): internal corruption of phar "%sfopen_edgecases.2.phar.php" (truncated manifest at stub end) in %sfopen_edgecases.php on line %d

Warning: unlink(): phar error: unlink failed in %sfopen_edgecases.php on line %d

Warning: unlink(): phar error: no directory in "phar://", must have at least phar:/// for root directory (always use full path to a new phar) in %sfopen_edgecases.php on line %d

Warning: unlink(): phar error: unlink failed in %sfopen_edgecases.php on line %d

Warning: unlink(): unable to open phar for reading "foo.phar" in %sfopen_edgecases.php on line %d

Warning: unlink(): phar error: unlink failed in %sfopen_edgecases.php on line %d

Warning: unlink(): phar error: "phar://test.phar/fopen_edgecases.php" cannot be unlinked, phar is extracted in plain map in %sfopen_edgecases.php on line %d

Warning: unlink(): unlink of "phar://%sfopen_edgecases.phar.php/oops" failed, file does not exist in %sfopen_edgecases.php on line %d

Warning: rename(): phar error: cannot rename "phar://" to "phar://": invalid url "phar://" in %sfopen_edgecases.php on line %d

Warning: rename(): phar error: cannot rename "phar://%sfopen_edgecases.phar.php/hi" to "phar://": invalid url "phar://" in %sfopen_edgecases.php on line %d

Warning: rename(): phar error: cannot rename "phar://foo.phar/hi" to "phar://": invalid url "phar://foo.phar/hi" in %sfopen_edgecases.php on line %d

Warning: rename(): phar error: cannot rename "phar://%sfopen_edgecases.phar.php/hi" to "phar://foo.phar/hi", not within the same phar archive in %sfopen_edgecases.php on line %d

Warning: rename(): phar error: write operations disabled by INI setting in %sfopen_edgecases.php on line %d

Warning: rename(): phar error: cannot rename "phar://test.phar/fopen_edgecases.php" to "phar://test.phar/hi" from extracted phar archive in %sfopen_edgecases.php on line %d

===DONE===