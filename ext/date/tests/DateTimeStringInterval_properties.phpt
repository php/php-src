--TEST--
Test DateTimeStringInterval readonly properties
--FILE--
<?php

$i = new DateTimeStringInterval("yesterday");

try {
    $i->from_string = false;
} catch (Error $e) {
    echo $e->getMessage() . "\n";
}

try {
    $i->date_string = "";
} catch (Error $e) {
    echo $e->getMessage() . "\n";
}

var_dump($i);

?>
--EXPECTF--
Cannot modify readonly property DateTimeStringInterval::$from_string
Cannot modify readonly property DateTimeStringInterval::$date_string
object(DateTimeStringInterval)#%d (%d) {
  ["from_string"]=>
  bool(true)
  ["date_string"]=>
  string(9) "yesterday"
}
