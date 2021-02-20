--TEST--
Implicit float to int conversions when float too large should warn, array variant
--FILE--
<?php

$float = 10e120;
$string_float = (string) $float;

$string = 'Here is some text for good measure';

try {
    echo 'Float casted to string compile', \PHP_EOL;
    $string[(string) 10e120] = 'E';
    var_dump($string);
} catch (\TypeError) {
    echo 'TypeError', \PHP_EOL;
}

?>
--EXPECTF--
Float casted to string compile

Warning: Uncaught TypeError: Cannot access offset of type string on string in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d

Fatal error: Allowed memory size of %d bytes exhausted %S(tried to allocate %d bytes) in %s on line %d
