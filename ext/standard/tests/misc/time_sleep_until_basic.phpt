--TEST--
time_sleep_until() function - basic test for time_sleep_until()
--CREDITS--
Manuel Baldassarri mb@ideato.it
Michele Orselli mo@ideato.it
#PHPTestFest Cesena Italia on 2009-06-20
--FILE--
<?php
  $time = time() + 2;
  time_sleep_until( $time );
  var_dump( time() >= $time );
?>
--EXPECT--
bool(true)
