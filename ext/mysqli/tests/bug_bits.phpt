--TEST--
Bug (Incorrectly decoding bit values / Malformed server packet. Field length pointing)
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifconnectfailure.inc');
?>
--FILE--
<?php

require_once("connect.inc");
if (!$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket)) {
    printf("[001] Connect failed, [%d] %s\n", mysqli_connect_errno(), mysqli_connect_error());
}

if (!$link->query("DROP TABLE IF EXISTS bug_bits")) {
    printf("[002] [%d] %s\n", $link->errno, $link->error);
}

if (!$link->query("CREATE TABLE `bug_bits` (`inty` bigint(20) unsigned NOT NULL DEFAULT '0', `bitty` bit(64) NOT NULL DEFAULT b'0')")) {
    printf("[003] [%d] %s\n", $link->errno, $link->error);
}

$insertQuery = "INSERT INTO `bug_bits` VALUES (18446744073709551615, 18446744073709551615)".
			   ",(18446744073709551614, 18446744073709551614)".
			   ",(4294967296, 4294967296)".
			   ",(4294967295, 4294967295)".
			   ",(2147483648, 2147483648)".
			   ",(2147483647, 2147483647)".
			   ",(1, 1)";
if (!$link->query($insertQuery)) {
    printf("[004] [%d] %s\n", $link->errno, $link->error);
}

if (!($res = $link->query("SELECT * FROM `bug_bits`"))) {
    printf("[005] [%d] %s\n", $link->errno, $link->error);
}

while ($row = $res->fetch_assoc()) {
	var_dump($row);
}

$link->close();

echo "Done\n";	
?>
--CLEAN--
<?php
require_once("connect.inc");
if (!$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket))
    printf("[c001] [%d] %s\n", mysqli_connect_errno(), mysqli_connect_error());

if (!mysqli_query($link, "DROP TABLE IF EXISTS bug_bits"))
    printf("[c002] Cannot drop table, [%d] %s\n", mysqli_errno($link), mysqli_error($link));

mysqli_close($link);
?>
--EXPECT--
array(2) {
  ["inty"]=>
  string(20) "18446744073709551615"
  ["bitty"]=>
  string(20) "18446744073709551615"
}
array(2) {
  ["inty"]=>
  string(20) "18446744073709551614"
  ["bitty"]=>
  string(20) "18446744073709551614"
}
array(2) {
  ["inty"]=>
  string(10) "4294967296"
  ["bitty"]=>
  string(10) "4294967296"
}
array(2) {
  ["inty"]=>
  string(10) "4294967295"
  ["bitty"]=>
  string(10) "4294967295"
}
array(2) {
  ["inty"]=>
  string(10) "2147483648"
  ["bitty"]=>
  string(10) "2147483648"
}
array(2) {
  ["inty"]=>
  string(10) "2147483647"
  ["bitty"]=>
  string(10) "2147483647"
}
array(2) {
  ["inty"]=>
  string(1) "1"
  ["bitty"]=>
  string(1) "1"
}
Done
