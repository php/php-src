--TEST--
show bounded autoload
--FILE--
<?php

namespace test;

spl_autoload_register(function($name) {
    echo "name=$name\n";
}, true, false, SPL_AUTOLOAD_FUNCTION);

echo strlen('foo');
echo strlen('bar');
echo strlen('baz');
?>
--EXPECT--
name=test\strlen
333
