--TEST--
Hash: hash_equals() test
--FILE--
<?php

function trycatch_dump(...$tests) {
    foreach ($tests as $test) {
        try {
            var_dump($test());
        }
        catch (\Error $e) {
            echo '[' . get_class($e) . '] ' . $e->getMessage() . "\n";
        }
    }
}

trycatch_dump(
    fn() => hash_equals("same", "same"),
    fn() => hash_equals("not1same", "not2same"),
    fn() => hash_equals("short", "longer"),
    fn() => hash_equals("longer", "short"),
    fn() => hash_equals("", "notempty"),
    fn() => hash_equals("notempty", ""),
    fn() => hash_equals("", ""),
    fn() => hash_equals(123, "NaN"),
    fn() => hash_equals("NaN", 123),
    fn() => hash_equals(123, 123),
    fn() => hash_equals(null, ""),
    fn() => hash_equals(null, 123),
    fn() => hash_equals(null, null),
);

?>
--EXPECT--
bool(true)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(true)
[TypeError] hash_equals(): Argument #1 ($known_string) must be of type string, int given
[TypeError] hash_equals(): Argument #2 ($user_string) must be of type string, int given
[TypeError] hash_equals(): Argument #1 ($known_string) must be of type string, int given
[TypeError] hash_equals(): Argument #1 ($known_string) must be of type string, null given
[TypeError] hash_equals(): Argument #1 ($known_string) must be of type string, null given
[TypeError] hash_equals(): Argument #1 ($known_string) must be of type string, null given
