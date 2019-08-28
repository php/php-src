--TEST--
Testing parameter type-hinted with default value inside namespace
--FILE--
<?php

namespace foo;

class bar {
	public function __construct(\StdClass $x = NULL) {
		var_dump($x);
	}
}

new bar(new \StdClass);
new bar(null);

?>
--EXPECTF--
object(StdClass)#%d (0) {
}
NULL
