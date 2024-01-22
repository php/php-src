--TEST--
Bug #77935: Crash in mysqlnd_fetch_stmt_row_cursor when calling an SP with a cursor
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
$db = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket);
$db->query('DROP PROCEDURE IF EXISTS testSp');
$db->query(<<<'SQL'
CREATE
    PROCEDURE `testSp`()
	BEGIN
		DECLARE `cur` CURSOR FOR SELECT 1;
		OPEN `cur`;
		CLOSE `cur`;
		SELECT 1;
	END;
SQL);

$stmt = $db->prepare("CALL testSp()");
$stmt->execute();
$result = $stmt->get_result();
while ($row = $result->fetch_assoc()) {
    var_dump($row);
}

?>
--CLEAN--
<?php
require_once 'connect.inc';
$link = new mysqli($host, $user, $passwd, $db, $port, $socket);
$link->query('DROP PROCEDURE IF EXISTS testSp');
$link->close();
?>
--EXPECT--
array(1) {
  [1]=>
  int(1)
}
