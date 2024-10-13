--TEST--
Test error operation of password_hash() with Yescrypt
--FILE--
<?php
try {
    password_hash('test', PASSWORD_YESCRYPT, ['block_count' => 3]);
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    password_hash('test', PASSWORD_YESCRYPT, ['block_count' => -1]);
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    password_hash('test', PASSWORD_YESCRYPT, ['block_count' => []]);
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    password_hash('test', PASSWORD_YESCRYPT, ['block_size' => 0]);
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    password_hash('test', PASSWORD_YESCRYPT, ['block_size' => []]);
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    password_hash('test', PASSWORD_YESCRYPT, ['parallelism' => 0]);
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    password_hash('test', PASSWORD_YESCRYPT, ['parallelism' => []]);
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    password_hash('test', PASSWORD_YESCRYPT, ['time' => -1]);
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    password_hash('test', PASSWORD_YESCRYPT, ['time' => []]);
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}

?>
--EXPECT--
Parameter "block_count" must be between 4 and 4294967295
Parameter "block_count" must be between 4 and 4294967295
Parameter "block_count" cannot be converted to int
Parameter "block_size" must be greater than 0
Parameter "block_size" cannot be converted to int
Parameter "parallelism" must be greater than 0
Parameter "parallelism" cannot be converted to int
Parameter "time" must be greater than or equal to 0
Parameter "time" cannot be converted to int
