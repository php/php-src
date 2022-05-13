--TEST--
mysqli_fetch_column()
--EXTENSIONS--
mysqli
--SKIPIF--
<?php
require_once 'skipifconnectfailure.inc';
if ($IS_MYSQLND) {
    die("skip libmysql only test");
}
?>
--FILE--
<?php

require_once "connect.inc";
require 'table.inc';
mysqli_report(MYSQLI_REPORT_ERROR | MYSQLI_REPORT_STRICT);

$res = mysqli_query($link, "SELECT id, label FROM test ORDER BY id LIMIT 1");

print "[001]\n";
var_dump(mysqli_fetch_column($res));

print "[002]\n";
var_dump(mysqli_fetch_column($res));


$res = mysqli_query($link, "SELECT
    1 AS a,
    2 AS a
");
print "[003]\n";
var_dump(mysqli_fetch_column($res, 0));

$res = mysqli_query($link, "SELECT
    1 AS a,
    2 AS a
");
print "[004]\n";
var_dump(mysqli_fetch_column($res, 1));

$res = mysqli_query($link, "SELECT
    1 AS a,
    2 AS a,
    3
");
print "[005]\n";
var_dump(mysqli_fetch_column($res, 2));

$res = mysqli_query($link, "SELECT
    1 AS a,
    2 AS a,
    3,
    NULL AS d
");
print "[006]\n";
var_dump(mysqli_fetch_column($res, 3));

$res = mysqli_query($link, "SELECT
    1 AS a,
    2 AS a,
    3,
    NULL AS d,
    true AS e
");
print "[007]\n";
var_dump(mysqli_fetch_column($res, 4));

$res = mysqli_query($link, "SELECT
    1.42 AS a
");
print "[008]\n";
var_dump(mysqli_fetch_column($res, 0));

$res = mysqli_query($link, "SELECT
    1.42E0 AS a
");
print "[009]\n";
var_dump(mysqli_fetch_column($res, 0));

$res = mysqli_query($link, "SELECT id, label FROM test ORDER BY id LIMIT 1");
print "[010]\n";
try {
    var_dump(mysqli_fetch_column($res, -1));
} catch (\ValueError $e) {
    echo $e->getMessage(), \PHP_EOL;
}

$res = mysqli_query($link, "SELECT id, label FROM test ORDER BY id LIMIT 1");
print "[011]\n";
try {
    var_dump(mysqli_fetch_column($res, 2));
} catch (\ValueError $e) {
    echo $e->getMessage(), \PHP_EOL;
}

mysqli_free_result($res);
try {
	mysqli_fetch_column($res);
} catch (Error $exception) {
	echo $exception->getMessage() . "\n";
}

$res = $link->query("SELECT id, label FROM test ORDER BY id LIMIT 2");

print "[012]\n";
var_dump($res->fetch_column());

print "[013]\n";
var_dump($res->fetch_column(1));

mysqli_close($link);
?>
--CLEAN--
<?php
require_once "clean_table.inc";
?>
--EXPECT--
[001]
string(1) "1"
[002]
bool(false)
[003]
string(1) "1"
[004]
string(1) "2"
[005]
string(1) "3"
[006]
NULL
[007]
string(1) "1"
[008]
string(4) "1.42"
[009]
string(4) "1.42"
[010]
mysqli_fetch_column(): Argument #2 ($column) must be greater than or equal to 0
[011]
mysqli_fetch_column(): Argument #2 ($column) must be less than the number of fields for this result set
mysqli_result object is already closed
[012]
string(1) "1"
[013]
string(1) "b"
