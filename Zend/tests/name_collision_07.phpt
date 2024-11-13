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
Fatal error: Cannot redeclare class Bazzle\Bar (previously declared as local import) in %s on line %d
