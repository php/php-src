--TEST--
Bug #35143 (gettimeofday() ignores current time zone)
--FILE--
<?php
date_default_timezone_set("UTC");

var_dump(date_default_timezone_get());
var_dump(gettimeofday());
?>
--EXPECTF--
unicode(3) "UTC"
array(4) {
  [u"sec"]=>
  int(%d)
  [u"usec"]=>
  int(%d)
  [u"minuteswest"]=>
  int(0)
  [u"dsttime"]=>
  int(0)
}
