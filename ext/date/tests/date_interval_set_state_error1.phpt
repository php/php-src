--TEST--
Test that DateInterval::__unserialize() doesn't modify state in case of a date_string format error
--FILE--
<?php

$interval = new DateInterval("P1Y1DT1H1M");

var_dump($interval);

try {
    $interval->__unserialize(
        [
            "date_string" => "wrong",
        ]
    );
} catch (Error $e) {
    echo $e->getMessage() . "\n";
}

var_dump($interval);

?>
--EXPECTF--
object(DateInterval)#%d (%d) {
  ["y"]=>
  int(1)
  ["m"]=>
  int(0)
  ["d"]=>
  int(1)
  ["h"]=>
  int(1)
  ["i"]=>
  int(1)
  ["s"]=>
  int(0)
  ["f"]=>
  float(0)
  ["invert"]=>
  int(0)
  ["days"]=>
  bool(false)
  ["from_string"]=>
  bool(false)
}
Unknown or bad format (wrong) at position 0 (w) while unserializing: The timezone could not be found in the database
object(DateInterval)#%d (%d) {
  ["y"]=>
  int(1)
  ["m"]=>
  int(0)
  ["d"]=>
  int(1)
  ["h"]=>
  int(1)
  ["i"]=>
  int(1)
  ["s"]=>
  int(0)
  ["f"]=>
  float(0)
  ["invert"]=>
  int(0)
  ["days"]=>
  bool(false)
  ["from_string"]=>
  bool(false)
}
