--TEST--
Using "readonly" as a function in use statements was never supported
--FILE--
<?php

namespace Foo\Bar {
    use function strlen as readonly;
}

namespace {
    use function strlen as readonly;
}

?>
--EXPECTF--
Parse error: syntax error, unexpected token "readonly", expecting identifier in %s on line %d
