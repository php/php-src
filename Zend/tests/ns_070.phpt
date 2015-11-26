--TEST--
Testing parameter type-hinted with default value inside namespace
--FILE--
<?php

namespace foo;

class bar {
	public function __construct(\stdclass $x = NULL) {
		var_dump($x);
	}
}

new bar(new \stdclass);
new bar(null);

?>
--EXPECTF--
object(stdClass)#%d (0) {
}
NULL
