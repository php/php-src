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
Deprecated: Increment on non-alphanumeric string is deprecated
string(0) ""
Deprecated: Decrement on empty string is deprecated as non-numeric
string(0) ""
Deprecated: Increment on non-alphanumeric string is deprecated
string(1) " "
Deprecated: Decrement on non-numeric string has no effect and is deprecated
string(1) " "
string(4) "199B"
Deprecated: Decrement on non-numeric string has no effect and is deprecated
string(4) "199B"
string(4) "A200"
Deprecated: Decrement on non-numeric string has no effect and is deprecated
string(4) "A200"
string(4) "200A"
Deprecated: Decrement on non-numeric string has no effect and is deprecated
string(4) "200A"
string(4) "Z200"
Deprecated: Decrement on non-numeric string has no effect and is deprecated
string(4) "Z200"
Deprecated: Increment on non-alphanumeric string is deprecated
string(11) "Hello world"
Deprecated: Decrement on non-numeric string has no effect and is deprecated
string(11) "Hello world"
Deprecated: Increment on non-alphanumeric string is deprecated
string(4) "🐘"
Deprecated: Decrement on non-numeric string has no effect and is deprecated
string(4) "🐘"
