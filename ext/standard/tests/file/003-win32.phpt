--TEST--
is_*() and file_exists() return values are boolean.
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) != 'WIN') {
    die('skip only for Windows');
}
?>
--POST--
--GET--
--FILE--
<?php

$funcs = array(
    'is_writable',
    'is_readable',
    'is_file',
    'file_exists',
);

$filename="";

foreach ($funcs as $test) {
    $bb = $test($filename);
    echo gettype($bb)."\n";
    clearstatcache();
}

$filename="run-tests.php";

foreach ($funcs as $test) {
    $bb = $test($filename);
    echo gettype($bb)."\n";
    clearstatcache();
}

?>
--EXPECT--
boolean
boolean
boolean
boolean
boolean
boolean
boolean
boolean
