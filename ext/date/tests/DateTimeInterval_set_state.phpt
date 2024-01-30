--TEST--
Test DateTimeInterval::__set_state()
--FILE--
<?php

var_dump(DateTimeInterval::__set_state([
    "y" => 2024,
]));

try {
    DateTimeInterval::__set_state([
        "date_string" => "yesterday"
    ]);
} catch (ValueError $e) {
    echo $e->getMessage() . "\n";
}

?>
--EXPECTF--
object(DateTimeInterval)#%d (%d) {
  ["from_string"]=>
  bool(false)
  ["y"]=>
  int(2024)
  ["m"]=>
  int(-1)
  ["d"]=>
  int(-1)
  ["h"]=>
  int(-1)
  ["i"]=>
  int(-1)
  ["s"]=>
  int(-1)
  ["f"]=>
  float(0)
  ["invert"]=>
  int(0)
  ["days"]=>
  int(-1)
}
DateInterval::__set_state(): Argument #1 ($array) cannot contain a "date_string" key with a string value
