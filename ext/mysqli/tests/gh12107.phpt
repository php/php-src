--TEST--
GH-12107 (When running a stored procedure (that returns a result set) twice, PHP crashes)
--EXTENSIONS--
mysqli
--SKIPIF--
<?php
require_once 'skipifconnectfailure.inc';
?>
--FILE--
<?php
require_once 'connect.inc';

$mysqli = new mysqli("$host:$port", $user, $passwd, $db);

$sql = <<<SQL
CREATE PROCEDURE `gh12107`()
BEGIN
    SELECT "hello world";
END;
SQL;
$mysqli->query($sql);

echo "Start or run 1\n";
$stmt = $mysqli->prepare("call `gh12107`()");
$stmt->execute();
$stmt->bind_result($output);
var_dump($stmt->fetch());
var_dump($output);
unset($output);
echo "End of run 1\n";

echo "Start or run 2\n";
$stmt->execute();
$stmt->bind_result($output);
var_dump($stmt->fetch());
var_dump($output);
echo "End of run 2\n";

?>
--CLEAN--
<?php
require_once 'connect.inc';
if (!$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket))
   printf("[c001] [%d] %s\n", mysqli_connect_errno(), mysqli_connect_error());

if (!mysqli_query($link, "DROP PROCEDURE IF EXISTS gh12107"))
    printf("[c002] Cannot drop procedure, [%d] %s\n", mysqli_errno($link), mysqli_error($link));

mysqli_close($link);
?>
--EXPECT--
Start or run 1
bool(true)
string(11) "hello world"
End of run 1
Start or run 2
bool(true)
string(11) "hello world"
End of run 2
