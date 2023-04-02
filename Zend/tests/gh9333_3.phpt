--TEST--
GH-9333: Allow catching "Non-abstract method must contain body" compile error
--FILE--
<?php
try {
    include __DIR__ . '/gh9333_3.inc';
} catch (Throwable $e) {
    echo get_class($e), ': ', $e->getMessage(), "\n";
}
echo (new ReflectionClass(Valid::class))->getName(), "\n";
try {
    echo (new ReflectionClass(Invalid::class))->getName(), "\n";
} catch (Throwable $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
CompileError: Non-abstract method Invalid::foo() must contain body
Valid
Class "Invalid" does not exist
