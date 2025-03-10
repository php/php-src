--TEST--
`never` parameter types - cannot be used for parameters on methods with implementations
--FILE--
<?php

abstract class Demo {
    public function example(never $v) {
        // Nothing
    }
}

?>
--EXPECTF--
Fatal error: never cannot be used as a parameter type for methods with implementations in %s on line %d
