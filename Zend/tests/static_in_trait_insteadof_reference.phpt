--TEST--
Cannot use static in trait insteadof method reference
--FILE--
<?php

trait SomeTrait {
    public function foobar() {}
}

class Test {
    use SomeTrait {
        static::foobar insteadof SomeTrait;
    }
}

?>
--EXPECTF--
Fatal error: Cannot use 'static' as trait name, as it is reserved in %s on line %d
