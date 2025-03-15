--TEST--
`never` parameter types - cannot be used for parameters with defaults
--FILE--
<?php

interface Demo {
    public function example(never $v = 5);
}

?>
--EXPECTF--
Fatal error: never cannot be used as a parameter type for parameters with defaults in %s on line %d
