--TEST--
--FILE--
<?php

$global = 42;

$func = function($x, stdClass $y=NULL) use($global) {
	static $static;
};

ReflectionFunction::Export($func);

$r = new ReflectionFunction($func);

var_dump(@get_class($r->getClosureThis()));
var_dump($r->getName());
var_dump($r->isClosure());

Class Test {
	public $func;
	function __construct(){
		global $global;
		$this->func = function($x, stdClass $y = NULL) use($global) {
			static $static;
		};
	}
}

ReflectionMethod::export(new Test, "func");

$r = new ReflectionMethod(new Test, "func");

var_dump(get_class($r->getClosureThis()));
var_dump($r->getName());
var_dump($r->isClosure());

?>
===DONE===
--EXPECTF--
Closure [ <user> function {closure} ] {
  @@ %s027.php 5 - 7

  - Static Parameters [2] {
    Parameter #0 [ $global ]
    Parameter #1 [ $static ]
  }

  - Parameters [2] {
    Parameter #0 [ <required> $x ]
    Parameter #1 [ <optional> stdClass or NULL $y = NULL ]
  }
}

bool(false)
unicode(9) "{closure}"
bool(true)
Closure [ <user> public method func ] {
  @@ %s027.php 21 - 23

  - Static Parameters [3] {
    Parameter #0 [ Test $this ]
    Parameter #1 [ $global ]
    Parameter #2 [ $static ]
  }

  - Parameters [2] {
    Parameter #0 [ <required> $x ]
    Parameter #1 [ <optional> stdClass or NULL $y = NULL ]
  }
}

unicode(4) "Test"
unicode(4) "func"
bool(true)
===DONE===
