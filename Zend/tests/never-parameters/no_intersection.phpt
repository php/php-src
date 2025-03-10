--TEST--
`never` parameter types - cannot be part of an intersection
--FILE--
<?php

interface Demo {
    public function example(never&Example $v);
}

?>
--EXPECTF--
Fatal error: Type never cannot be part of an intersection type in %s on line %d
