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

try {
	$result = compact(true);
} catch (TypeError $e) {
	echo "TypeError: ".$e->getMessage()."\n";
}

try {
	$foo = 'bar';
	$bar = 'baz';
	$result = compact($foo, [42]);
} catch (TypeError $e) {
	echo "TypeError: ".$e->getMessage();
}
?>
--EXPECTF--
Warning: compact(): Undefined variable $c\u0327ity in %s on line %d
array(2) {
  ["event"]=>
  string(8) "SIGGRAPH"
  ["state"]=>
  string(2) "CA"
}
TypeError: compact(): Argument #1 ($var_name) must be string or array of strings, bool given
TypeError: compact(): Argument #2 must be string or array of strings, int given
