--TEST--
Bug GH-10218 (DateTimeZone fails to parse time zones that contain the "+" character)
--FILE--
<?php
var_dump(new DateTime('now', new DateTimeZone('Etc/GMT+1')));
?>
--EXPECTF--
object(DateTime)#%d (%d) {
  ["date"]=>
  string(%d) "%s"
  ["timezone_type"]=>
  int(3)
  ["timezone"]=>
  string(9) "Etc/GMT+1"
}
