--TEST--
Bug #80837 Calling stmt_store_result after fetch doesn't throw an error
--EXTENSIONS--
mysqli
--SKIPIF--
<?php
require_once 'skipifconnectfailure.inc';
if (!defined('MYSQLI_STORE_RESULT_COPY_DATA')) die('skip requires mysqlnd');
?>
--FILE--
<?php
require_once 'connect.inc';

mysqli_report(MYSQLI_REPORT_ERROR | MYSQLI_REPORT_STRICT);
$mysqli = new my_mysqli($host, $user, $passwd, $db, $port, $socket);

$mysqli->query('DROP TABLE IF EXISTS test');
$mysqli->query('CREATE TABLE test (b int)');
$mysqli->query('INSERT INTO test VALUES (1),(2),(3)');

$statement = $mysqli->prepare("SELECT b FROM test");
$statement->execute();
$statement->bind_result($name);
$statement->fetch();
try {
    $statement->store_result();
} catch (mysqli_sql_exception $e) {
    echo $e->getMessage();
}

$mysqli->close();

?>
--CLEAN--
<?php
require_once 'clean_table.inc';
?>
--EXPECT--
Commands out of sync; you can't run this command now
