--TEST--
Using "_" as a function in use statements is NOT deprecated
--FILE--
<?php

namespace Foo\Bar {
    use function strlen as _;
}

namespace {
    use function strlen as _;

    echo "OK";
}

?>
--EXPECT--
OK
