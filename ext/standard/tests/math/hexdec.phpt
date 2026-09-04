--TEST--
overflow check for _php_math_basetozval
--INI--
precision=14
--FILE--
<?php

var_dump(hexdec("012345"));
var_dump(hexdec("12345"));

try {
    var_dump(hexdec("q12345"));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage() . "\n";
}

try {
    var_dump(hexdec("12345+?!"));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage() . "\n";
}

try {
    var_dump(hexdec("12345q"));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage() . "\n";
}

var_dump((float)hexdec("1234500001"));
var_dump((float)hexdec("17fffffff"));

?>
--EXPECT--
int(74565)
int(74565)
Exception: Invalid characters passed for attempted conversion
Exception: Invalid characters passed for attempted conversion
Exception: Invalid characters passed for attempted conversion
float(78187069441)
float(6442450943)
