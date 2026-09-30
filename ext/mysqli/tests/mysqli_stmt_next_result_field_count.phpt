--TEST--
mysqli_stmt::$field_count is 0 for the trailing OK packet of a stored procedure
--EXTENSIONS--
mysqli
--SKIPIF--
<?php
require_once 'skipifconnectfailure.inc';
?>
--FILE--
<?php
require_once 'connect.inc';

mysqli_report(MYSQLI_REPORT_ERROR | MYSQLI_REPORT_STRICT);
$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket);
$link->query('DROP PROCEDURE IF EXISTS test_field_count_p');
$link->query('CREATE PROCEDURE test_field_count_p() BEGIN SELECT 1 AS a; SELECT 2 AS b, 3 AS c; END');

$stmt = $link->prepare('CALL test_field_count_p()');
$stmt->execute();
do {
    var_dump($stmt->field_count);
    $stmt->get_result();
} while ($stmt->next_result());
?>
--CLEAN--
<?php
require_once 'connect.inc';
$link = new mysqli($host, $user, $passwd, $db, $port, $socket);
$link->query('DROP PROCEDURE IF EXISTS test_field_count_p');
?>
--EXPECT--
int(1)
int(2)
int(0)
