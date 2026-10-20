--TEST--
Indirect call with empty class and/or method name.
--FILE--
<?php
class TestClass
{
    public static function __callStatic($method, array $args)
    {
        var_dump($method);
    }
}

// Test call using array syntax
$callback = ['TestClass', ''];
$callback();

// Test call using Class::method syntax.
$callback = 'TestClass::';
$callback();

// Test array syntax with empty class name
$callback = ['', 'method'];
try {
    $callback();
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

// Test Class::method syntax with empty class name
$callback = '::method';
try {
    $callback();
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

// Test array syntax with empty class and method name
$callback = ['', ''];
try {
    $callback();
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

// Test Class::method syntax with empty class and method name
$callback = '::';
try {
    $callback();
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

// Test string ending in single colon
$callback = 'Class:';
try {
    $callback();
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

// Test string beginning in single colon
$callback = ':method';
try {
    $callback();
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

// Test single colon
$callback = ':';
try {
    $callback();
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
?>
--EXPECT--
string(0) ""
string(0) ""
Error: Class "" not found
Error: Class "" not found
Error: Class "" not found
Error: Class "" not found
Error: Call to undefined function Class:()
Error: Call to undefined function :method()
Error: Call to undefined function :()
