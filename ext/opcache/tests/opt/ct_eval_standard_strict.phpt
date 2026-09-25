--TEST--
Compile-time evaluation preserves strict parameter types
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
--FILE--
<?php
declare(strict_types=1);
$calls = [
    fn() => str_increment(123),
    fn() => str_decrement(123),
    fn() => chop(123),
    fn() => join(123, ['a', 'b']),
    fn() => strchr('123', 2),
];
foreach ($calls as $call) {
    try {
        $call();
        echo "Unexpected success\n";
    } catch (TypeError $e) {
        echo $e->getMessage(), "\n";
    }
}
?>
--EXPECT--
str_increment(): Argument #1 ($string) must be of type string, int given
str_decrement(): Argument #1 ($string) must be of type string, int given
chop(): Argument #1 ($string) must be of type string, int given
join(): Argument #1 ($separator) must be of type array|string, int given
strchr(): Argument #2 ($needle) must be of type string, int given
