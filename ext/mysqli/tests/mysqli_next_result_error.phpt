--TEST--
Error in multi query
--EXTENSIONS--
mysqli
--SKIPIF--
<?php
require_once 'skipifconnectfailure.inc';
?>
--FILE--
<?php

require_once __DIR__ . '/connect.inc';

mysqli_report(MYSQLI_REPORT_ERROR | MYSQLI_REPORT_STRICT);

$mysqli = new mysqli($host, $user, $passwd, $db, $port, $socket);

$mysqli->multi_query("SELECT 1; SELECT 2; Syntax Error");

try {
    do {
        if ($res = $mysqli->store_result()) {
            var_dump($res->fetch_all(MYSQLI_ASSOC));
            $res->free();
        }
    } while ($mysqli->more_results() && $mysqli->next_result());
} catch (mysqli_sql_exception $e) {
    echo $e->getMessage(), "\n";
}

$mysqli->query("DROP PROCEDURE IF EXISTS p");
$mysqli->query('CREATE PROCEDURE p() READS SQL DATA BEGIN SELECT 1; SELECT foobar FROM table_that_does_not_exist; END;');

$stmt = $mysqli->prepare("CALL p()");
$stmt->execute();

try {
    do {
        $stmt->bind_result($num);
        while ($stmt->fetch()) {
            echo "num = $num\n";
        }
    } while ($stmt->more_results() && $stmt->next_result());
} catch (mysqli_sql_exception $e) {
    echo $e->getMessage(), "\n";
}

$mysqli->query("DROP PROCEDURE IF EXISTS p");

?>
--EXPECTF--
array(1) {
  [0]=>
  array(1) {
    [1]=>
    string(1) "1"
  }
}
array(1) {
  [0]=>
  array(1) {
    [2]=>
    string(1) "2"
  }
}
You have an error in your SQL syntax; %s
num = 1
Table '%s.table_that_does_not_exist' doesn't exist
