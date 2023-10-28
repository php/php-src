--TEST--
Bug #49081 (DateTime::diff() mistake if start in January and interval > 28 days)
--FILE--
<?php
   date_default_timezone_set('Europe/Berlin');
   $d1 = new DateTime('2010-01-01 06:00:00');
   $d2 = new DateTime('2010-01-31 10:00:00');
   $d  = $d1->diff($d2);
   print_r($d);
?>
--EXPECT--
DateInterval Object
(
    [y] => 0
    [m] => 0
    [d] => 30
    [h] => 4
    [i] => 0
    [s] => 0
    [f] => 0
    [invert] => 0
    [days] => 30
    [from_string] => 
    [date_string] => 
)
