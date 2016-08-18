--TEST--
timezone_offset_get() tests
--FILE--
<?php
date_default_timezone_set('UTC');

$dto = date_create("2006-12-12");
var_dump($dto);

$dtz = date_timezone_get($dto);
var_dump($dtz);

var_dump(timezone_offset_get());
var_dump(timezone_offset_get($dtz, $dto));
var_dump(timezone_offset_get($dto, $dtz));

echo "Done\n";

?>
--EXPECTF-- 
object(DateTime)#%d (3) {
  ["date"]=>
  string(26) "2006-12-12 00:00:00.000000"
  ["timezone_type"]=>
  int(3)
  ["timezone"]=>
  string(3) "UTC"
}
object(DateTimeZone)#%d (2) {
  ["timezone_type"]=>
  int(3)
  ["timezone"]=>
  string(3) "UTC"
}

Warning: timezone_offset_get() expects exactly 2 parameters, 0 given in %s on line %d
bool(false)
int(0)

Fatal error: Uncaught TypeError: Argument 1 passed to timezone_offset_get() must be an instance of DateTimeZone, instance of DateTime given in %s:%d
Stack trace:
#0 %s(%d): timezone_offset_get(Object(DateTime), Object(DateTimeZone))
#1 {main}
  thrown in %s on line %d
