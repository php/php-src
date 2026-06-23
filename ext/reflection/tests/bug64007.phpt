--TEST--
Bug #64007 (There is an ability to create instance of Generator by hand)
--FILE--
<?php
$reflection = new ReflectionClass('Generator');
try {
    $generator = $reflection->newInstanceWithoutConstructor();
    var_dump($generator);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

try {
    $generator = $reflection->newInstance();
    var_dump($generator);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
?>
--EXPECT--
ReflectionException: The "Generator" class is reserved for internal use and cannot be manually instantiated
ReflectionException: The "Generator" class is reserved for internal use and cannot be manually instantiated
