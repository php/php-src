--TEST--
PS using cursor and returning multiple result sets
--EXTENSIONS--
mysqli
--SKIPIF--
<?php
require_once('skipifconnectfailure.inc');
?>
--FILE--
<?php
require_once(__DIR__ . '/connect.inc');

mysqli_report(MYSQLI_REPORT_ERROR | MYSQLI_REPORT_STRICT);
$db = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket);
$db->query('DROP PROCEDURE IF EXISTS testPs');
$db->query(<<<'SQL'
CREATE PROCEDURE testPs() BEGIN
    DECLARE testCursor CURSOR FOR SELECT 'stuff';
    OPEN testCursor;
    CLOSE testCursor;
    SELECT 1 as a, 2 as b;
    SELECT 3 as a, 4 as b;
END
SQL
);

echo "use_result:\n";
$stmt = $db->prepare("call testPs()");
$stmt->execute();
$stmt->bind_result($v1, $v2);
while ($stmt->fetch()) {
    var_dump($v1, $v2);
}

$stmt->next_result();
$stmt->bind_result($v1, $v2);
while ($stmt->fetch()) {
    var_dump($v1, $v2);
}
$stmt->next_result();

echo "\nstore_result:\n";
$stmt = $db->prepare("call testPs()");
$stmt->execute();
$stmt->store_result();
$stmt->bind_result($v1, $v2);
while ($stmt->fetch()) {
    var_dump($v1, $v2);
}

$stmt->next_result();
$stmt->store_result();
$stmt->bind_result($v1, $v2);
while ($stmt->fetch()) {
    var_dump($v1, $v2);
}
$stmt->next_result();

echo "\nget_result:\n";
$stmt = $db->prepare("call testPs()");
$stmt->execute();
$result = $stmt->get_result();
while ($row = $result->fetch_assoc()) {
    var_dump($row);
}

$stmt->next_result();
$result = $stmt->get_result();
while ($row = $result->fetch_assoc()) {
    var_dump($row);
}
$stmt->next_result();

?>
--CLEAN--
<?php
require_once 'connect.inc';
$link = new mysqli($host, $user, $passwd, $db, $port, $socket);
$link->query('DROP PROCEDURE IF EXISTS testPs');
$link->close();
?>
--EXPECT--
use_result:
int(1)
int(2)
int(3)
int(4)

store_result:
int(1)
int(2)
int(3)
int(4)

get_result:
array(2) {
  ["a"]=>
  int(1)
  ["b"]=>
  int(2)
}
array(2) {
  ["a"]=>
  int(3)
  ["b"]=>
  int(4)
}
