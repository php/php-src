--TEST--
`never` parameter types - cannot be used for parameters with defaults
--FILE--
<?php

interface Demo {
    public function example(never $v = 5);
}

?>
--EXPECTF--
Fatal error: Cannot use int as default value for parameter $v of type never in %s on line %d
