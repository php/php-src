--TEST--
compact()
--FILE--
<?php

$çity  = "San Francisco";
$state = "CA";
$event = "SIGGRAPH";

$location_vars = array("c\\u0327ity", "state");

$result = compact("event", $location_vars);
var_dump($result);
?>
--EXPECTF--
Notice: compact(): Undefined variable: c\u0327ity in %s on line %d
array(2) {
  ["event"]=>
  string(8) "SIGGRAPH"
  ["state"]=>
  string(2) "CA"
}
