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
string(%d) "%s"
string(%d) "%s"
int(%d)
string(21) "test\foo::__construct"
string(8) "test\foo"
string(11) "__construct"
string(4) "test"
