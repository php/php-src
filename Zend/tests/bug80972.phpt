--TEST--
Bug #80972: Memory exhaustion on invalid string offset
--FILE--
<?php

function exceptions_error_handler($severity, $message, $filename, $lineno) {
    if (error_reporting() & $severity) {
        throw new ErrorException($message, 0, $severity, $filename, $lineno);
    }
}
set_error_handler('exceptions_error_handler');

$float = 10e120;
$string_float = (string) $float;

$string = 'Here is some text for good measure';

try {
    echo 'Float casted to string compile', \PHP_EOL;
    $string[(string) 10e120] = 'E';
    var_dump($string);
} catch (\TypeError $e) {
    echo $e->getMessage(), \PHP_EOL;
}

/* This same bug also permits to modify the first byte of a string even if
 * the offset is invalid */
try {
    /* This must not affect the string value */
    $string["wrong"] = "f";
} catch (\Throwable $e) {
    echo $e->getMessage() . \PHP_EOL;
}
var_dump($string);

?>
--EXPECT--
Float casted to string compile
Cannot access offset of type string on string
Cannot access offset of type string on string
string(34) "Here is some text for good measure"
