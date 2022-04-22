--TEST--
Test DateInterval::__serialize and DateInterval::__unserialize
--FILE--
<?php
date_default_timezone_set("Europe/London");

$d = DateInterval::createFromDateString('next weekday 15:30');
echo "Original object:\n";
var_dump($d);

echo "\n\nSerialised object:\n";
try {
	$s = serialize($d);
} catch (Exception $e) {
	echo $e->getMessage(), "\n";
}
?>
--EXPECTF--
Original object:
object(DateInterval)#1 (10) {
  ["y"]=>
  int(0)
  ["m"]=>
  int(0)
  ["d"]=>
  int(0)
  ["h"]=>
  int(0)
  ["i"]=>
  int(0)
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


Serialised object:
Serializing special relative time specifications is not supported
