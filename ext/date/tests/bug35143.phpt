--TEST--
Bug #35143 (gettimeofday() ignores current time zone)
--FILE--
<?php
date_default_timezone_set("UTC");

var_dump(date_default_timezone_get());
var_dump(gettimeofday());
?>
--EXPECTF--
string(3) "UTC"
array(4) {
  ["sec"]=>
  int(%d)
  ["usec"]=>
  int(%d)
  ["minuteswest"]=>
  int(0)
  ["dsttime"]=>
  int(0)
}
--UEXPECTF--
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
