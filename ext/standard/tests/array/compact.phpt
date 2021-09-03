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

$result = compact(true);
$foo = 'bar';
$bar = 'baz';
$result = compact($foo, [42]);
var_dump($result);

?>
--EXPECTF--
Warning: compact(): Undefined variable $c\u0327ity in %s on line %d
array(2) {
  ["event"]=>
  string(8) "SIGGRAPH"
  ["state"]=>
  string(2) "CA"
}

Warning: compact(): Argument #1 must be string or array of strings, bool given in %s on line %d

Warning: compact(): Argument #2 must be string or array of strings, int given in %s on line %d
array(1) {
  ["bar"]=>
  string(3) "baz"
}