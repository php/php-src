--TEST--
namespace_declare(): Basic usage
--FILE--
<?php

namespace_declare('Foo', ['strict_types' => 1]);
namespace_declare('Foo\Bar', ['strict_types' => 0]);

function test($namespace, $extra = '') {
    echo $namespace, ": ";
    eval(<<<PHP
$extra
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
test('Foo\NotBar');
test('Foo\Bar\Baz');
test('Foobar');
test('foo');
test('foo\BAR');

test('Foo', 'declare(strict_types=0);');
test('Foo\Bar', 'declare(strict_types=1);');

?>
--EXPECT--
Foo: strlen() expects parameter 1 to be string, int given
Foo\Bar: 2
Foo\NotBar: strlen() expects parameter 1 to be string, int given
Foo\Bar\Baz: 2
Foobar: 2
foo: strlen() expects parameter 1 to be string, int given
foo\BAR: 2
Foo: 2
Foo\Bar: strlen() expects parameter 1 to be string, int given
