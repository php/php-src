--TEST--
SPL autoloader calls local functions first
--FILE--
<?php

namespace {
    spl_autoload_register(function(string $name) {
        echo "name=$name\n";
    }, true, false, SPL_AUTOLOAD_FUNCTION);
}

namespace Foo {
    echo strlen('foo') . "\n";
}
?>
--EXPECT--
name=foo\strlen
3
