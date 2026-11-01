--TEST--
Str prepared objects reject cloning, serialization and construction bypasses
--EXTENSIONS--
strmatch
--FILE--
<?php
foreach ([new Str\Matcher(['x']), new Str\Replacer(['x' => 'Y'])] as $object) {
    $class = $object::class;
    foreach ([
        static fn () => clone $object,
        static fn () => serialize($object),
        static fn () => unserialize(sprintf('O:%d:"%s":0:{}', strlen($class), $class)),
        static fn () => (new ReflectionClass($class))->newInstanceWithoutConstructor(),
        static fn () => (new ReflectionClass($class))->newLazyGhost(static function ($object) {}),
    ] as $operation) {
        try {
            $operation();
            echo "FAIL\n";
        } catch (Throwable $e) {
            echo $e::class, ': ', $e->getMessage(), "\n";
        }
    }
}
?>
--EXPECT--
Error: Trying to clone an uncloneable object of class Str\Matcher
Exception: Serialization of 'Str\Matcher' is not allowed
Exception: Unserialization of 'Str\Matcher' is not allowed
ReflectionException: Class Str\Matcher is an internal class marked as final that cannot be instantiated without invoking its constructor
Error: Cannot make instance of internal class lazy: Str\Matcher is internal
Error: Trying to clone an uncloneable object of class Str\Replacer
Exception: Serialization of 'Str\Replacer' is not allowed
Exception: Unserialization of 'Str\Replacer' is not allowed
ReflectionException: Class Str\Replacer is an internal class marked as final that cannot be instantiated without invoking its constructor
Error: Cannot make instance of internal class lazy: Str\Replacer is internal
