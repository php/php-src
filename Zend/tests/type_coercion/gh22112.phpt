--TEST--
GH-22112 (Assertion failure when error handler throws during NaN to bool/string coercion at function entry)
--FILE--
<?php

set_error_handler(function ($errno, $errstr) {
    throw new Exception($errstr);
});

function take_bool(bool $v) {
    echo "take_bool entered\n";
}

function take_string(string $v) {
    echo "take_string entered\n";
}

$nan = fdiv(0, 0);

try {
    take_bool($nan);
} catch (Exception $e) {
    echo 'bool: ', $e::class, ': ', $e->getMessage(), "\n";
}

try {
    take_string($nan);
} catch (Exception $e) {
    echo 'string: ', $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
bool: Exception: unexpected NAN value was coerced to bool
string: Exception: unexpected NAN value was coerced to string
