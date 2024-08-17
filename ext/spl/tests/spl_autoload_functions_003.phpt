--TEST--
SPL autoloader only autoloads a local function once
--FILE--
<?php

namespace {
    spl_autoload_register(function(string $name, int $mode) {
        echo "name=$name, mode=$mode\n";
    }, true, false, SPL_AUTOLOAD_FUNCTION);
}

namespace Foo {
    echo no_exist('foo') . "\n";
}
?>
--EXPECTF--
name=foo\no_exist, mode=2

Fatal error: Uncaught Error: Call to undefined function Foo\no_exist() in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
