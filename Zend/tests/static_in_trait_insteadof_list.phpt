--TEST--
Cannot use static in trait insteadof list
--FILE--
<?php

trait SomeTrait {
    public function foobar() {}
}

class Test {
    use SomeTrait {
        SomeTrait::foobar insteadof static;
    }
}

?>
--EXPECTF--
Fatal error: Cannot use 'static' as trait name, as it is reserved in %s on line %d
