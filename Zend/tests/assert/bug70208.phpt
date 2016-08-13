--TEST--
Bug #70208 (scope information must be preserved with assert())
--FILE--
<?php

function non_class_scope() {
	return true;
}

class test {
	protected $prop = 1;

	public function __construct() {
		assert('non_class_scope();');
		var_dump($this->prop);
	}
}

new test;

?>
--EXPECT--
int(1)
