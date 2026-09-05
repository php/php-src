--TEST--
Test password_hash() - argument errors
--FILE--
<?php

foreach ([
    fn() => password_hash('password'),
    fn() => password_hash([], PASSWORD_BCRYPT),
    fn() => password_hash('password', []),
    fn() => password_hash('password', PASSWORD_BCRYPT, 'options'),
] as $callback) {
    try {
        $callback();
    } catch (Throwable $e) {
        echo $e::class, ': ', $e->getMessage(), "\n";
    }
}

?>
--EXPECT--
ArgumentCountError: password_hash() expects at least 2 arguments, 1 given
TypeError: password_hash(): Argument #1 ($password) must be of type string, array given
TypeError: password_hash(): Argument #2 ($algo) must be of type string|int|null, array given
TypeError: password_hash(): Argument #3 ($options) must be of type array, string given
