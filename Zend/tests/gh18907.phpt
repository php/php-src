--TEST--
GH-18907: Leak when creating cycle inside hook
--FILE--
<?php

class Foo {
    public $prop {
        get {
            $this->prop = $this;
            return 1;
        }
    }
}

function test() {
    var_dump((new Foo)->prop);
}

/* Call twice to test the ZEND_IS_PROPERTY_HOOK_SIMPLE_GET() path. */
test();
test();

?>
--EXPECT--
int(1)
int(1)
