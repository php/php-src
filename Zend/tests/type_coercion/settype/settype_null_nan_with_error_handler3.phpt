--TEST--
Error handler dtor NAN value, set to null 3
--FILE--
<?php

set_error_handler(function ($errno, $errstr) {
    global $nan;
    $nan = bin2hex(random_bytes(4));
    echo $errstr, "\n";
});

$nan = fdiv(0, 0);
var_dump($nan);
settype($nan, 'null');
var_dump($nan);

?>
--EXPECT--
float(NAN)
unexpected NAN value was coerced to null
NULL
