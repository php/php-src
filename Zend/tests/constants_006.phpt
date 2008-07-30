--TEST--
Magic constants lowercased
--FILE--
<?php

namespace test;

var_dump(__dir__);
var_dump(__file__);
var_dump(__line__);

class foo {
	public function __construct() {
		var_dump(__method__);
		var_dump(__class__);
		var_dump(__function__);
	}
}

new foo;

var_dump(__namespace__);

?>
--EXPECTF--
unicode(%d) "%s"
unicode(%d) "%s"
int(%d)
unicode(22) "test::foo::__construct"
unicode(9) "test::foo"
unicode(11) "__construct"
unicode(4) "test"
