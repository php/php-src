--TEST--
Bug #41523 (strtotime('0000-00-00 00:00:00') is parsed as 1999-11-30) (64 bit)
--SKIPIF--
<?php echo PHP_INT_SIZE != 8 ? "skip 64-bit only" : "OK"; ?>
--FILE--
<?php
date_default_timezone_set("UTC");

var_dump( date_parse('0000-00-00 00:00:00') );
var_dump( strtotime('0000-00-00 00:00:00') );
var_dump( $dt = new DateTime('0000-00-00 00:00:00') );
echo $dt->format( DateTime::ISO8601 ), "\n";

?>
--EXPECT--
array(12) {
  ["year"]=>
  int(0)
  ["month"]=>
  int(0)
  ["day"]=>
  int(0)
  ["hour"]=>
  int(0)
  ["minute"]=>
  int(0)
  ["second"]=>
  int(0)
  ["fraction"]=>
  float(0)
  ["warning_count"]=>
  int(0)
  ["warnings"]=>
  array(0) {
  }
  ["error_count"]=>
  int(0)
  ["errors"]=>
  array(0) {
  }
  ["is_localtime"]=>
  bool(false)
}
int(-62169984000)
object(DateTime)#1 (0) {
}
-0001-11-30T00:00:00+0000
