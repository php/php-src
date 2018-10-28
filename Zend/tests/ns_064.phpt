--TEST--
Magic methods in overridden stdClass inside namespace
--FILE--
<?php

namespace test;

class foo {
	public $e = array();

	public function __construct() {
		$this->e[] = $this;
	}

	public function __set($a, $b) {
		var_dump($a, $b);
	}
	public function __get($a) {
		var_dump($a);
		return $this;
	}
}

use test\foo as stdClass;

$x = new stdClass;
$x->a = 1;
$x->b->c = 1;
$x->d->e[0]->f = 2;

?>
--EXPECT--
string(1) "a"
int(1)
string(1) "b"
string(1) "c"
int(1)
string(1) "d"
string(1) "f"
int(2)
