--TEST--
Reserved keywords in namespace name
--FILE--
<?php

namespace iter\fn {
    function test() {
        echo __FUNCTION__, "\n";
    }
}

namespace fn {
    function test() {
        echo __FUNCTION__, "\n";
    }
}

namespace self {
    function test() {
        echo __FUNCTION__, "\n";
    }
}

namespace {
    use iter\fn;
    use function fn\test as test2;
    use function self\test as test3;
    fn\test();
    test2();
    test3();
}

?>
--EXPECT--
iter\fn\test
fn\test
self\test
