--TEST--
GH-23232 (lone namespace separator asks the autoloader for an empty class name)
--FILE--
<?php
spl_autoload_register(function (string $class): void {
    echo "autoload: '$class'\n";
});

foreach (['::a', '\::a', '\Foo::a', 'Foo::a'] as $callable) {
    echo "is_callable(\"$callable\")\n";
    var_dump(is_callable($callable));
}

var_dump(class_exists('\\'));
?>
--EXPECT--
is_callable("::a")
bool(false)
is_callable("\::a")
bool(false)
is_callable("\Foo::a")
autoload: 'Foo'
bool(false)
is_callable("Foo::a")
autoload: 'Foo'
bool(false)
bool(false)
