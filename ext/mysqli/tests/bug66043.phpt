--TEST--
Bug #66043 (Segfault calling bind_param() on mysqli)
--SKIPIF--
<?php
require_once('skipif.inc');
require_once("connect.inc");
if ($IS_MYSQLND) {
    die("skip libmysql only test");
}
require_once('skipifconnectfailure.inc');
?>
--FILE--
<?php
require 'connect.inc';
if (!$db = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket)) {
	printf("[001] Connect failed, [%d] %s\n", mysqli_connect_errno(), mysqli_connect_error());
}

$stmt = $db->stmt_init();
$stmt->prepare("SELECT User FROM user WHERE password=\"\"");
$stmt->execute();
$stmt->bind_result($testArg);
echo "Okey";
?>
--EXPECTF--
Okey
