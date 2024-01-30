--TEST--
Test DateTimeStringInterval::__set_state()
--FILE--
<?php

var_dump(DateTimeStringInterval::__set_state([
    "date_string" => "yesterday"
]));

try {
    DateTimeStringInterval::__set_state([
        "y" => 2024,
    ]);
} catch (ValueError $e) {
    echo $e->getMessage() . "\n";
}

?>
--EXPECTF--
object(DateTimeStringInterval)#%d (%d) {
  ["from_string"]=>
  bool(true)
  ["date_string"]=>
  string(9) "yesterday"
}
DateInterval::__set_state(): Argument #1 ($array) must contain a "date_string" key with a string value
