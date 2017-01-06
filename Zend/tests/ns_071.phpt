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

Fatal error: Uncaught TypeError: Argument 1 passed to foo\bar::__construct() must be of the type array or null, object given, called in %s on line %d and defined in %s:%d
Stack trace:
#0 %s(%d): foo\bar->__construct(Object(stdClass))
#1 {main}
  thrown in %s on line %d
