--TEST--
Bug #52808 (Segfault when specifying interval as two dates)
--FILE--
<?php
date_default_timezone_set('Europe/Oslo');
$intervals = array(
	/* Three correct sets*/
    "2008-05-11T15:30:00Z/2007-03-01T13:00:00Z",
    "2007-05-11T15:30:00Z/2008-03-01T13:00:00Z",
    "2007-05-11T15:30:00Z 2008-03-01T13:00:00Z",
	/* Error situations */
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
    catch ( \DateMalformedIntervalStringException $e )
    {
        echo $e::class, ': ', $e->getMessage(), "\n";
    }
}
echo "==DONE==\n";
?>
--EXPECTF--
object(DateInterval)#%d (%d) {
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
  ["invert"]=>
  int(1)
  ["days"]=>
  int(437)
  ["from_string"]=>
  bool(false)
}
object(DateInterval)#%d (%d) {
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
  ["invert"]=>
  int(0)
  ["days"]=>
  int(294)
  ["from_string"]=>
  bool(false)
}
object(DateInterval)#%d (%d) {
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
  ["invert"]=>
  int(0)
  ["days"]=>
  int(294)
  ["from_string"]=>
  bool(false)
}
DateMalformedIntervalStringException: Failed to parse interval (2007-05-11T15:30:00Z/)
DateMalformedIntervalStringException: Failed to parse interval (2007-05-11T15:30:00Z)
DateMalformedIntervalStringException: Unknown or bad format (2007-05-11T15:30:00Z/:00Z)
==DONE==
