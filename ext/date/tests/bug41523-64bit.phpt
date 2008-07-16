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
  [u"year"]=>
  int(0)
  [u"month"]=>
  int(0)
  [u"day"]=>
  int(0)
  [u"hour"]=>
  int(0)
  [u"minute"]=>
  int(0)
  [u"second"]=>
  int(0)
  [u"fraction"]=>
  float(0)
  [u"warning_count"]=>
  int(1)
  [u"warnings"]=>
  array(1) {
    [20]=>
    unicode(27) "The parsed date was invalid"
  }
  [u"error_count"]=>
  int(0)
  [u"errors"]=>
  array(0) {
  }
  [u"is_localtime"]=>
  bool(false)
}
int(-62169984000)
object(DateTime)#1 (3) {
  [u"date"]=>
  unicode(20) "-0001-11-30 00:00:00"
  [u"timezone_type"]=>
  int(3)
  [u"timezone"]=>
  unicode(3) "UTC"
}
-0001-11-30T00:00:00+0000
