--TEST--
GH-16910 (Fractional seconds truncated in prepared statements if decimals is AUTO_SEC_PART_DIGITS)
--EXTENSIONS--
mysqli
--SKIPIF--
<?php
require_once 'connect.inc';
if (!$link = @my_mysqli_connect($host, $user, $passwd, $db, $port, $socket))
    die(sprintf("SKIP Cannot connect to the server using host=%s, user=%s, passwd=***, dbname=%s, port=%s, socket=%s\n",
        $host, $user, $db, $port, $socket));
$stmt = $link->prepare("SELECT FROM_UNIXTIME('1992.1')");
if (!$stmt || $stmt->result_metadata()->fetch_field_direct(0)->decimals !== 39)
    die("SKIP Server does not report AUTO_SEC_PART_DIGITS for FROM_UNIXTIME()");
?>
--FILE--
<?php
require_once 'connect.inc';

mysqli_report(MYSQLI_REPORT_ERROR | MYSQLI_REPORT_STRICT);
$link = new my_mysqli($host, $user, $passwd, $db, $port, $socket);
$link->query("SET time_zone = '+00:00'");

$sql = "SELECT FROM_UNIXTIME('1992.1'), FROM_UNIXTIME('1992.0')";
var_dump($link->query($sql)->fetch_row());

$stmt = $link->prepare($sql);
$stmt->execute();
var_dump($stmt->get_result()->fetch_row());
?>
--EXPECT--
array(2) {
  [0]=>
  string(26) "1970-01-01 00:33:12.100000"
  [1]=>
  string(19) "1970-01-01 00:33:12"
}
array(2) {
  [0]=>
  string(26) "1970-01-01 00:33:12.100000"
  [1]=>
  string(19) "1970-01-01 00:33:12"
}
