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
  int(1)
  ["warnings"]=>
  array(1) {
    [20]=>
    string(27) "The parsed date was invalid"
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
object(DateTime)#1 (3) {
  ["date"]=>
  string(20) "-0001-11-30 00:00:00"
  ["timezone_type"]=>
  int(3)
  ["timezone"]=>
  string(3) "UTC"
}
-0001-11-30T00:00:00+0000
