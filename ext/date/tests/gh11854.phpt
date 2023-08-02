--TEST--
GH-11854 (8.2.9 DateTime:createFromFormat stopped parsing datetime with containing extra space.)
--FILE--
<?php
$dateTime = \DateTime::createFromFormat("D M d H:i:s Y", "Wed Aug  2 08:37:50 2023");
var_dump($dateTime);
?>
--EXPECT--
object(DateTime)#1 (3) {
  ["date"]=>
  string(26) "2023-08-02 08:37:50.000000"
  ["timezone_type"]=>
  int(3)
  ["timezone"]=>
  string(3) "UTC"
}
