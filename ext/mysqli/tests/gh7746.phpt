--TEST--
Bug #GH-7746 mysqli_sql_exception->sqlstate is inaccessible
--EXTENSIONS--
mysqli
--SKIPIF--
<?php
require_once 'skipifconnectfailure.inc';
?>
--FILE--
<?php
require 'connect.inc';
$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket);

mysqli_report(MYSQLI_REPORT_ERROR | MYSQLI_REPORT_STRICT);

try {
    $link->query("stuff");
} catch (mysqli_sql_exception $exception) {
    var_dump($exception->getSqlState());
}

?>
--EXPECT--
string(5) "42000"
