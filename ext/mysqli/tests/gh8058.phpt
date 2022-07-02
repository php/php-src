--TEST--
GH-8058 (NULL pointer dereference in mysqlnd package (#81706))
--EXTENSIONS--
mysqli
--SKIPIF--
<?php
require_once 'skipifconnectfailure.inc';
?>
--FILE--
<?php
require_once "connect.inc";

mysqli_report(MYSQLI_REPORT_OFF);
$mysqli = new my_mysqli($host, $user, $passwd, $db, $port, $socket);

// There should be no segfault due to NULL deref
$stmt = $mysqli->prepare("select 1,2,3");
$stmt->bind_result($a,$a,$a);
$stmt->prepare("");
$stmt->prepare("select ".str_repeat("'A',", 0x1201)."1");
unset($stmt); // trigger dtor

// There should be no memory leak
$stmt = $mysqli->prepare("select 1,2,3");
$stmt->bind_result($a,$a,$a);
$stmt->prepare("");
$stmt->prepare("select 1");
unset($stmt); // trigger dtor

mysqli_report(MYSQLI_REPORT_ERROR | MYSQLI_REPORT_STRICT);
$stmt = $mysqli->prepare("select 1,2,3");
try {
    // We expect an exception to be thrown
    $stmt->prepare("");
} catch (mysqli_sql_exception $e) {
    var_dump($e->getMessage());
}
?>
--EXPECT--
string(15) "Query was empty"
