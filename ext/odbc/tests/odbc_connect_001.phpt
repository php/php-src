--TEST--
odbc_connect(): Basic test for odbc_connect(). (When not using a DSN alias)
--EXTENSIONS--
odbc
--SKIPIF--
<?php
include 'config.inc';
if (strpos($dsn, '=') === false) {
    die('skip uses a DSN alias');
}
include 'skipif.inc';
?>
--XLEAK--
A bug in msodbcsql causes a memory leak when reconnecting after closing. See GH-12306
--FILE--
<?php

include 'config.inc';
$dsn = str_replace(";uid={$user};pwd={$pass}", '', $dsn);

echo "dsn without credentials / correct user / correct password\n";
$conn = odbc_connect($dsn, $user, $pass);
echo $conn ? "Connected.\n\n" : "";
if ($conn) odbc_close($conn);

echo "dsn with correct user / incorrect user / correct password\n";
$conn = odbc_connect("{$dsn};uid={$user}", 'hoge', $pass);
echo $conn ? "Connected.\n\n" : "";
if ($conn) odbc_close($conn);

echo "dsn with correct password / correct user / incorrect password\n";
$conn = odbc_connect("{$dsn};PWD={$pass}", $user, 'fuga');
echo $conn ? "Connected.\n\n" : "";
if ($conn) odbc_close($conn);

echo "dsn with correct credentials / incorrect user / incorrect password\n";
$conn = odbc_connect("{$dsn};Uid={$user};pwD={$pass}", 'hoge', 'fuga');
echo $conn ? "Connected.\n\n" : "";
if ($conn) odbc_close($conn);

echo "dsn with correct credentials / null user / null password\n";
$conn = odbc_connect("{$dsn};Uid={$user};pwD={$pass}", null, null);
echo $conn ? "Connected.\n\n" : "";
if ($conn) odbc_close($conn);

echo "dsn with correct credentials / not set user / not set password\n";
$conn = odbc_connect("{$dsn};Uid={$user};pwD={$pass}");
echo $conn ? "Connected.\n" : "";
if ($conn) odbc_close($conn);
?>
--EXPECT--
dsn without credentials / correct user / correct password
Connected.

dsn with correct user / incorrect user / correct password
Connected.

dsn with correct password / correct user / incorrect password
Connected.

dsn with correct credentials / incorrect user / incorrect password
Connected.

dsn with correct credentials / null user / null password
Connected.

dsn with correct credentials / not set user / not set password
Connected.
