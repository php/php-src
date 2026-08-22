--TEST--
Test password_needs_rehash() - argument errors
--FILE--
<?php

foreach ([
    fn() => password_needs_rehash('hash'),
    fn() => password_needs_rehash([], PASSWORD_BCRYPT),
    fn() => password_needs_rehash('hash', []),
    fn() => password_needs_rehash('hash', PASSWORD_BCRYPT, 'options'),
] as $callback) {
    try {
        $callback();
    } catch (Throwable $e) {
        echo $e::class, ': ', $e->getMessage(), "\n";
    }
}

?>
--EXPECT--
ArgumentCountError: password_needs_rehash() expects at least 2 arguments, 1 given
TypeError: password_needs_rehash(): Argument #1 ($hash) must be of type string, array given
TypeError: password_needs_rehash(): Argument #2 ($algo) must be of type string|int|null, array given
TypeError: password_needs_rehash(): Argument #3 ($options) must be of type array, string given
