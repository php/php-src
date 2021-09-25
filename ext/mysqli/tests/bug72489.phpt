--TEST--
Bug #72489 (PHP Crashes When Modifying Array Containing MySQLi Result Data)
--EXTENSIONS--
mysqli
--SKIPIF--
<?php
require_once('skipifconnectfailure.inc');
?>
--FILE--
<?php

require_once("connect.inc");
if (!$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket)) {
    printf("[001] Connect failed, [%d] %s\n", mysqli_connect_errno(), mysqli_connect_error());
}

if (!$link->query("DROP TABLE IF EXISTS bug72489")) {
    printf("[002] [%d] %s\n", $link->errno, $link->error);
}

if (!$link->query("CREATE TABLE bug72489 (id INT(6) UNSIGNED AUTO_INCREMENT PRIMARY KEY, code VARCHAR(30) NOT NULL)")) {
    printf("[003] [%d] %s\n", $link->errno, $link->error);
}

$seedSQL = "INSERT INTO bug72489 (`code`) VALUES ('code1'), ('code2'), ('code3');";
if (!$link->query($seedSQL)) {
    printf("[004] [%d] %s\n", $link->errno, $link->error);
}

$subRow = array();

if (!$stmt = $link->prepare("SELECT id, code FROM bug72489")) {
    printf("[005] [%d] %s\n", $link->errno, $link->error);
}

$stmt->attr_set(MYSQLI_STMT_ATTR_CURSOR_TYPE, MYSQLI_CURSOR_TYPE_READ_ONLY);
if (!$stmt->bind_result($subRow['id'], $subRow['code']) || !$stmt->execute()) {
    printf("[006] [%d] %s\n", $link->errno, $link->error);
}

while ($stmt->fetch()) {
    $testArray = array('id' => 1);
    $subRow['code'] = $testArray;
}

echo "Finished 1\n";

$newArray = array();

echo "Finished 2\n";

?>
--CLEAN--
<?php
require_once("connect.inc");
if (!$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket))
    printf("[c001] [%d] %s\n", mysqli_connect_errno(), mysqli_connect_error());

if (!mysqli_query($link, "DROP TABLE IF EXISTS bug72489"))
    printf("[c002] Cannot drop table, [%d] %s\n", mysqli_errno($link), mysqli_error($link));

mysqli_close($link);
?>
--EXPECT--
Finished 1
Finished 2
