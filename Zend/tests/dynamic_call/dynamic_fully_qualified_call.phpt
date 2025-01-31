--TEST--
Crash when using dynamic call syntax with fully qualified name in a namespace
--FILE--
<?php

namespace Foo;
try {
    ('\bar')();
} catch (\Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
Call to undefined function bar()
