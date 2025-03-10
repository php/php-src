--TEST--
`never` parameter types - cannot be part of a union
--FILE--
<?php

interface Demo {
    public function example(never|Example $v);
}

?>
--EXPECTF--
Fatal error: never can only be used as a standalone type in %s on line %d
