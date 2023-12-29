--TEST--
Test DateTimeStringInterval::__construct()
--FILE--
<?php

var_dump(new DateTimeStringInterval("yesterday"));

try {
    new DateTimeStringInterval("");
} catch (DateException $e) {
    echo $e->getMessage() . "\n";
}

try {
    new DateTimeStringInterval("P1D");
} catch (DateException $e) {
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
Unknown or bad format () at position 0 ( ): Empty string
Unknown or bad format (P1D) at position 1 (1): Unexpected character
