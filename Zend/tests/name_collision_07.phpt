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
Fatal error: Class Bazzle\Bar cannot be declared because the name is already in use in %s on line %d
