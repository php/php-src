--TEST--
Cannot use $this as lexical variable
--FILE--
<?php

class Foo {
    public function fn() {
        return function() use ($this) {};
    }
}

?>
--EXPECTF--
Fatal error: Cannot use $this as lexical variable in %s on line %d
