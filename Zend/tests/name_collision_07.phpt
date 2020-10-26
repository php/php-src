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
?>
--EXPECTF--
Fatal error: Class Bazzle\Bar has already been declared in %s on line %d
