--TEST--
It does not matter whether declares for child namespaces are added first
--FILE--
<?php

namespace_declare('Foo\Bar\Baz', ['strict_types' => 1]);
namespace_declare('Foo\Bar', ['strict_types' => 0]);
namespace_declare('Foo', ['strict_types' => 1]);

function test($namespace) {
    echo $namespace, ": ";
    eval(<<<PHP
namespace $namespace;

try {
    echo strlen(10), "\n";
} catch (\Error \$e) {
    echo \$e->getMessage(), "\n";
}
PHP
    );
}

test('Foo');
test('Foo\Bar');
test('Foo\Bar\Baz');

?>
--EXPECT--
Foo: strlen() expects parameter 1 to be string, integer given
Foo\Bar: 2
Foo\Bar\Baz: strlen() expects parameter 1 to be string, integer given
