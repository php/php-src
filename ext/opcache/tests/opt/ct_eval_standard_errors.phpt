--TEST--
Compile-time evaluation preserves runtime errors from standard functions
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
--FILE--
<?php
set_error_handler(function ($severity, $message) {
    echo "Warning: $message\n";
});
$calls = [
    fn() => str_increment(''),
    fn() => str_decrement('0'),
    fn() => chop('abc', 'z..a'),
    fn() => join('-', 'abc'),
    fn() => strchr([], 'a'),
];
echo "Runtime\n";
foreach ($calls as $call) {
    try {
        $call();
    } catch (Throwable $e) {
        echo $e::class, ': ', $e->getMessage(), "\n";
    }
}
?>
--EXPECT--
Runtime
ValueError: str_increment(): Argument #1 ($string) must not be empty
ValueError: str_decrement(): Argument #1 ($string) "0" is out of decrement range
Warning: chop(): Invalid '..'-range, '..'-range needs to be incrementing
TypeError: join(): Argument #2 ($array) must be of type ?array, string given
TypeError: strchr(): Argument #1 ($haystack) must be of type string, array given
