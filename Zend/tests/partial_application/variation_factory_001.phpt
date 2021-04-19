--TEST--
Partial application variation internal object factory
--FILE--
<?php
$date = 
    new DateTime(?, timezone: new DateTimeZone("Asia/Tokyo"));

var_dump($date("now")->getTimeZone());
?>
--EXPECTF--
object(DateTimeZone)#%d (2) {
  ["timezone_type"]=>
  int(3)
  ["timezone"]=>
  string(10) "Asia/Tokyo"
}

