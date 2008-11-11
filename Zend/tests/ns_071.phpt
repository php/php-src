--TEST--
Testing parameter type-hinted (array) with default value inside namespace
--FILE--
<?php

namespace foo;

class bar {
	public function __construct(array $x = NULL) {
		var_dump($x);
	}
}

new bar(null);
new bar(new \stdclass);

?>
--EXPECTF--
NULL

Catchable fatal error: Argument 1 passed to foo\bar::__construct() must be an array, object given, called in %s on line %d and defined in %s on line %d
