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

function inc(&$v) {
    $v++;
}
function dec(&$v) {
    $v--;
}

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
    $v = $value;
    try {
        inc($v);
    } catch (\Exception $e) {
        echo $e->getMessage(), PHP_EOL;
    }
    var_dump($v);
    $v = $value;
    try {
        dec($v);
    } catch (\Exception $e) {
        echo $e->getMessage(), PHP_EOL;
    }
    var_dump($v);
}
?>
--EXPECT--
Deprecated: Increment on non-numeric string is deprecated, use str_increment() instead
string(1) "1"
Deprecated: Decrement on empty string is deprecated as non-numeric
int(-1)
Deprecated: Increment on non-numeric string is deprecated, use str_increment() instead
string(1) " "
Deprecated: Decrement on non-numeric string has no effect and is deprecated
string(1) " "
Deprecated: Increment on non-numeric string is deprecated, use str_increment() instead
string(4) "199B"
Deprecated: Decrement on non-numeric string has no effect and is deprecated
string(4) "199A"
Deprecated: Increment on non-numeric string is deprecated, use str_increment() instead
string(4) "A200"
Deprecated: Decrement on non-numeric string has no effect and is deprecated
string(4) "A199"
Deprecated: Increment on non-numeric string is deprecated, use str_increment() instead
string(4) "200A"
Deprecated: Decrement on non-numeric string has no effect and is deprecated
string(4) "199Z"
Deprecated: Increment on non-numeric string is deprecated, use str_increment() instead
string(4) "Z200"
Deprecated: Decrement on non-numeric string has no effect and is deprecated
string(4) "Z199"
Deprecated: Increment on non-numeric string is deprecated, use str_increment() instead
string(11) "Hello worle"
Deprecated: Decrement on non-numeric string has no effect and is deprecated
string(11) "Hello world"
Deprecated: Increment on non-numeric string is deprecated, use str_increment() instead
string(4) "🐘"
Deprecated: Decrement on non-numeric string has no effect and is deprecated
string(4) "🐘"
