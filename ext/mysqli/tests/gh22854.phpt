--TEST--
GH-22854 (Assertion failure / crash at link_errno_read in ext/mysqli/mysqli_prop.c)
--EXTENSIONS--
mysqli
--FILE--
<?php

$mysqli = new mysqli();
try {
    @$mysqli->connect("doesnotexist");
} catch (Throwable $e) {
}

try {
    var_dump($mysqli->errno);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

try {
    var_dump($mysqli->error);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

var_dump($mysqli);

?>
--EXPECTF--
mysqli object is not fully initialized
mysqli object is not fully initialized
object(mysqli)#%d (4) {
  ["client_info"]=>
  string(%d) "%s"
  ["client_version"]=>
  int(%d)
  ["connect_errno"]=>
  int(%d)
  ["connect_error"]=>
  string(%d) "%s"
}
