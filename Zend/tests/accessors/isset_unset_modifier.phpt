--TEST--
Isset and unset can have modifiers
--FILE--
<?php

class Test {
    public $foo {
	    final isset;
		final unset;
    }
}

class Test2 extends Test {
    public $foo {
	    isset;
		unset;
    }
}

?>
--EXPECTF--
Fatal error: Cannot override final accessor Test::$foo->isset() in %s on line %d
