--TEST--
SPL autoloader calls local functions first
--FILE--
<?php

namespace {
    spl_autoload_register(function(string $name, int $mode) {
        echo "name=$name, mode=$mode\n";
    }, true, false, SPL_AUTOLOAD_FUNCTION);
}

namespace Foo {
    echo strlen('foo') . "\n";
}
?>
--EXPECT--
name=foo\strlen, mode=2
3
