--TEST--
Test gettimeofday() function : basic functionality
--FILE--
<?php
echo "*** Testing gettimeofday() : basic functionality ***\n";

date_default_timezone_set("Asia/Calcutta");

// Calling gettimeofday() with all possible arguments
var_dump( gettimeofday(true) );

var_dump( gettimeofday() );

var_dump( gettimeofday(false) );

?>
--EXPECTF--
*** Testing gettimeofday() : basic functionality ***
float(%f)
array(4) {
  ["sec"]=>
  int(%d)
  ["usec"]=>
  int(%d)
  ["minuteswest"]=>
  int(-330)
  ["dsttime"]=>
  int(0)
}
array(4) {
  ["sec"]=>
  int(%d)
  ["usec"]=>
  int(%d)
  ["minuteswest"]=>
  int(-330)
  ["dsttime"]=>
  int(0)
}
