--TEST--
Bug #52808 (Segfault when specifying interval as two dates)
--FILE--
<?php
date_default_timezone_set('Europe/Oslo');
$intervals = array(
	"2008-05-11T15:30:00Z/2007-03-01T13:00:00Z",
	"2007-05-11T15:30:00Z/2008-03-01T13:00:00Z",
	"2007-05-11T15:30:00Z 2008-03-01T13:00:00Z",
	"2007-05-11T15:30:00Z/",
	"2007-05-11T15:30:00Z",
	"2007-05-11T15:30:00Z/:00Z",
);
foreach($intervals as $iv) {
    try
    {
    	$di = new DateInterval($iv);
    	var_dump($di);
    }
    catch ( Exception $e )
    {
    	echo $e->getMessage(), "\n";
    }
}
echo "==DONE==\n";
?>
--EXPECTF--
object(DateInterval)#%d (16) {
  ["y"]=>
  int(1)
  ["m"]=>
  int(2)
  ["d"]=>
  int(10)
  ["h"]=>
  int(2)
  ["i"]=>
  int(30)
  ["s"]=>
  int(0)
  ["f"]=>
  float(0)
  ["weekday"]=>
  int(0)
  ["weekday_behavior"]=>
  int(0)
  ["first_last_day_of"]=>
  int(0)
  ["invert"]=>
  int(1)
  ["days"]=>
  int(437)
  ["special_type"]=>
  int(0)
  ["special_amount"]=>
  int(0)
  ["have_weekday_relative"]=>
  int(0)
  ["have_special_relative"]=>
  int(0)
}
object(DateInterval)#%d (16) {
  ["y"]=>
  int(0)
  ["m"]=>
  int(9)
  ["d"]=>
  int(18)
  ["h"]=>
  int(21)
  ["i"]=>
  int(30)
  ["s"]=>
  int(0)
  ["f"]=>
  float(0)
  ["weekday"]=>
  int(0)
  ["weekday_behavior"]=>
  int(0)
  ["first_last_day_of"]=>
  int(0)
  ["invert"]=>
  int(0)
  ["days"]=>
  int(294)
  ["special_type"]=>
  int(0)
  ["special_amount"]=>
  int(0)
  ["have_weekday_relative"]=>
  int(0)
  ["have_special_relative"]=>
  int(0)
}
object(DateInterval)#%d (16) {
  ["y"]=>
  int(0)
  ["m"]=>
  int(9)
  ["d"]=>
  int(18)
  ["h"]=>
  int(21)
  ["i"]=>
  int(30)
  ["s"]=>
  int(0)
  ["f"]=>
  float(0)
  ["weekday"]=>
  int(0)
  ["weekday_behavior"]=>
  int(0)
  ["first_last_day_of"]=>
  int(0)
  ["invert"]=>
  int(0)
  ["days"]=>
  int(294)
  ["special_type"]=>
  int(0)
  ["special_amount"]=>
  int(0)
  ["have_weekday_relative"]=>
  int(0)
  ["have_special_relative"]=>
  int(0)
}
DateInterval::__construct(): Failed to parse interval (2007-05-11T15:30:00Z/)
DateInterval::__construct(): Failed to parse interval (2007-05-11T15:30:00Z)
DateInterval::__construct(): Unknown or bad format (2007-05-11T15:30:00Z/:00Z)
==DONE==
