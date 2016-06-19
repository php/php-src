--TEST--
Class declaration colliding with import (in namespace)
--FILE--
<?php

namespace Foo {
    class Bar {}
}

namespace Bazzle {
    use Foo\Bar;
    class Bar {}
}
--EXPECTF--
Fatal error: Cannot declare class Bazzle\Bar because the name is already in use in %s on line %d
