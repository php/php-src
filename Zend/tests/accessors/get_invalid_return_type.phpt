--TEST--
get accessor may not have return type
--FILE--
<?php

class Test {
    public int $prop {
        get(): int { return 42; }
    }
}

?>
--EXPECTF--
Fatal error: Accessor "get" may not have a return type (accessor types are determined by the property type) in %s on line %d
