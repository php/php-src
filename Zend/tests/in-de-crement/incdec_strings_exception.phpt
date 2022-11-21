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
    } catch (\Exception $e) {
        echo $e->getMessage(), PHP_EOL;
    }
    var_dump($value);
    try {
        $value--;
    } catch (\Exception $e) {
        echo $e->getMessage(), PHP_EOL;
    }
    var_dump($value);
}
?>
--EXPECT--
Deprecated: Increment on non-numeric string is deprecated
string(0) ""
Deprecated: Decrement on empty string is deprecated as non-numeric
string(0) ""
Deprecated: Increment on non-numeric string is deprecated
string(1) " "
Warning: Decrement on non-numeric string has no effect
string(1) " "
Deprecated: Increment on non-numeric string is deprecated
string(4) "199A"
Warning: Decrement on non-numeric string has no effect
string(4) "199A"
Deprecated: Increment on non-numeric string is deprecated
string(4) "A199"
Warning: Decrement on non-numeric string has no effect
string(4) "A199"
Deprecated: Increment on non-numeric string is deprecated
string(4) "199Z"
Warning: Decrement on non-numeric string has no effect
string(4) "199Z"
Deprecated: Increment on non-numeric string is deprecated
string(4) "Z199"
Warning: Decrement on non-numeric string has no effect
string(4) "Z199"
Deprecated: Increment on non-numeric string is deprecated
string(11) "Hello world"
Warning: Decrement on non-numeric string has no effect
string(11) "Hello world"
Deprecated: Increment on non-numeric string is deprecated
string(4) "🐘"
Warning: Decrement on non-numeric string has no effect
string(4) "🐘"
