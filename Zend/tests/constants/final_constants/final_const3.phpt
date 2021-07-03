--TEST--
Private class constants cannot be final
--FILE--
<?php

class Foo
{
    private final const A = "foo";
}

?>
--EXPECTF--
Fatal error: Private constant Foo::A cannot be final as it is not visible to other classes in %s on line %d
