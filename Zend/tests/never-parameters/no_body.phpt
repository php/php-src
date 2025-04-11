--TEST--
`never` parameter types - cannot be used for parameters on methods with bodies
--FILE--
<?php

abstract class Demo {
    public function example(never $v) {
        // Nothing
    }
}

?>
--EXPECTF--
Fatal error: Function Demo::example() containing a body cannot use never as a parameter type in %s on line %d
