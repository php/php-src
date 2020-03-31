--TEST--
Cannot add namespace_declare()s to namespace in current file
--FILE--
<?php

namespace Foo\Bar;
use Error;

try {
    namespace_declare('Foo\Bar', ['strict_types' => 1]);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
Cannot add declares to namespace "Foo\Bar", as it is already in use
