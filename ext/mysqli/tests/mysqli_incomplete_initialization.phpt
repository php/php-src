--TEST--
Incomplete initialization edge case where mysql->mysql is NULL
--EXTENSIONS--
mysqli
--FILE--
<?php

mysqli_report(MYSQLI_REPORT_OFF);
$mysqli = new mysqli();
@$mysqli->__construct('doesnotexist');
$mysqli->close();

?>
--EXPECTF--
Fatal error: Uncaught Error: mysqli object is not fully initialized in %s:%d
Stack trace:
#0 %s(%d): mysqli->close()
#1 {main}
  thrown in %s on line %d
