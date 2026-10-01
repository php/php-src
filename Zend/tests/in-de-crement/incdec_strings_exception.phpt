--TEST--
Inc/dec on string: warning/deprecations converted to exception
--FILE--
<?php

set_error_handler(function($severity, $m) {
    if ($severity == E_DEPRECATED) {
        $m = 'Deprecated: ' . $m;
    }
    if ($severity == E_WARNING) {
        $m = 'Warning: ' . $m;
    }
    throw new Exception($m, $severity);
});

$values = [
    '',
    ' ',
    // Alphanumeric values
    '199A',
    'A199',
    '199Z',
    'Z199',
    // Strings
    'Hello world',
    '🐘'
];
foreach ($values as $value) {
    try {
        $value++;
    } catch (\Throwable $e) {
        echo $e::class, ': ', $e->getMessage(), "\n";
    }
    var_dump($value);
    try {
        $value--;
    } catch (\Throwable $e) {
        echo $e::class, ': ', $e->getMessage(), "\n";
    }
    var_dump($value);
}
?>
--EXPECT--
Exception: Deprecated: Increment on non-numeric string is deprecated, use str_increment() instead
string(0) ""
Exception: Deprecated: Decrement on empty string is deprecated as non-numeric
string(0) ""
Exception: Deprecated: Increment on non-numeric string is deprecated, use str_increment() instead
string(1) " "
Exception: Deprecated: Decrement on non-numeric string has no effect and is deprecated
string(1) " "
Exception: Deprecated: Increment on non-numeric string is deprecated, use str_increment() instead
string(4) "199A"
Exception: Deprecated: Decrement on non-numeric string has no effect and is deprecated
string(4) "199A"
Exception: Deprecated: Increment on non-numeric string is deprecated, use str_increment() instead
string(4) "A199"
Exception: Deprecated: Decrement on non-numeric string has no effect and is deprecated
string(4) "A199"
Exception: Deprecated: Increment on non-numeric string is deprecated, use str_increment() instead
string(4) "199Z"
Exception: Deprecated: Decrement on non-numeric string has no effect and is deprecated
string(4) "199Z"
Exception: Deprecated: Increment on non-numeric string is deprecated, use str_increment() instead
string(4) "Z199"
Exception: Deprecated: Decrement on non-numeric string has no effect and is deprecated
string(4) "Z199"
Exception: Deprecated: Increment on non-numeric string is deprecated, use str_increment() instead
string(11) "Hello world"
Exception: Deprecated: Decrement on non-numeric string has no effect and is deprecated
string(11) "Hello world"
Exception: Deprecated: Increment on non-numeric string is deprecated, use str_increment() instead
string(4) "🐘"
Exception: Deprecated: Decrement on non-numeric string has no effect and is deprecated
string(4) "🐘"
