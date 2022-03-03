--TEST--
DBA check behaviour of key as an array with elements not convertable to string
--EXTENSIONS--
dba
--SKIPIF--
<?php
    require_once(__DIR__ .'/skipif.inc');
    die("info $HND handler used");
?>
--FILE--
<?php
require_once(__DIR__ .'/test.inc');
$db_file = dba_open($db_file, "n", $handler);

if ($db_file === false) {
    die('Error creating database');
}

/* Use an object */
$o = new stdClass();
try {
    var_dump(dba_insert([$o, 'obj'], 'Test', $db_file));
} catch (\Error $e) {
    echo $e::class, ': ', $e->getMessage(), \PHP_EOL;
}
try {
    var_dump(dba_insert(['group', $o], 'Test', $db_file));
} catch (\Error $e) {
    echo $e::class, ': ', $e->getMessage(), \PHP_EOL;
}

dba_close($db_file);

?>
--CLEAN--
<?php
    require(__DIR__ .'/clean.inc');
?>
--EXPECT--
Error: Object of class stdClass could not be converted to string
Error: Object of class stdClass could not be converted to string
