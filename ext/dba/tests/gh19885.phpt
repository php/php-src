--TEST--
GH-19885 (dba_fetch() segfault on large skip values)
--EXTENSIONS--
dba
--SKIPIF--
<?php
if (PHP_INT_SIZE != 8) die("skip this test is for 64bit platform only");
$handler = 'cdb';
require_once(__DIR__ .'/skipif.inc');
?>
--FILE--
<?php
$handler = 'cdb';
$db_file = __DIR__.'/test.cdb';
$db =dba_open($db_file, "r", $handler);
try {
	dba_fetch("1", $db, PHP_INT_MIN);
} catch (\ValueError $e) {
	echo $e->getMessage(), PHP_EOL;
}

try {
	dba_fetch("1", $db, PHP_INT_MAX);
} catch (\ValueError $e) {
	echo $e->getMessage(), PHP_EOL;
}
// negative skip needs to remain acceptable albeit corrected down the line
var_dump(dba_fetch("1", $db, -1000000));
?>
--EXPECTF--
dba_fetch(): Argument #3 ($skip) must be between -%d and %d
dba_fetch(): Argument #3 ($skip) must be between -%d and %d

Notice: dba_fetch(): Handler cdb accepts only skip values greater than or equal to zero, using skip=0 in %s on line %d
string(1) "1"
