--TEST--
Test error operation of password_hash() with Argon2i and Argon2id
--SKIPIF--
<?php
if (!defined('PASSWORD_ARGON2I')) die('skip password_hash not built with Argon2');
if (!defined('PASSWORD_ARGON2ID')) die('skip password_hash not built with Argon2');
if (PASSWORD_ARGON2_PROVIDER != 'standard') die('skip argon2 not provided by standard');
?>
--FILE--
<?php
try {
    password_hash('test', PASSWORD_ARGON2I, ['memory_cost' => 0]);
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    password_hash('test', PASSWORD_ARGON2I, ['time_cost' => 0]);
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    password_hash('test', PASSWORD_ARGON2I, ['threads' => 0]);
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    password_hash('test', PASSWORD_ARGON2ID, ['memory_cost' => 0]);
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    password_hash('test', PASSWORD_ARGON2ID, ['time_cost' => 0]);
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    password_hash('test', PASSWORD_ARGON2ID, ['threads' => 0]);
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}
?>
--EXPECT--
Memory cost is outside of allowed memory range
Time cost is outside of allowed time range
Invalid number of threads
Memory cost is outside of allowed memory range
Time cost is outside of allowed time range
Invalid number of threads
