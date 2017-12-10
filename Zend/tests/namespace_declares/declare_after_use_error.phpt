--TEST--
Cannot use namespace_declare() after namespace was used
--FILE--
<?php

eval(<<<'PHP'
namespace Foo\Bar;
PHP
);

try {
    namespace_declare('Foo\Bar', ['strict_types' => 1]);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

try {
    namespace_declare('Foo', ['strict_types' => 1]);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
Cannot add declares to namespace "Foo\Bar", as it is already in use
Cannot add declares to namespace "Foo", as it is already in use
