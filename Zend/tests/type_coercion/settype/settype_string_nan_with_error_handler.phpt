--TEST--
Error handler dtor NAN value, set to string
--FILE--
<?php

set_error_handler(function ($errno, $errstr) {
    global $nan;
    $nan = null;
    echo $errstr, "\n";
});

$nan = fdiv(0, 0);
var_dump($nan);
settype($nan, 'string');
var_dump($nan);

?>
--EXPECT--
float(NAN)
unexpected NAN value was coerced to string
string(3) "NAN"
