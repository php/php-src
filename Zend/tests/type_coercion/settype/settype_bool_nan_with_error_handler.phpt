--TEST--
Error handler dtor NAN value, set to bool
--FILE--
<?php

set_error_handler(function ($errno, $errstr) {
    global $nan;
    $nan = null;
    echo $errstr, "\n";
});

$nan = fdiv(0, 0);
var_dump($nan);
settype($nan, 'bool');
var_dump($nan);

?>
--EXPECT--
float(NAN)
unexpected NAN value was coerced to bool
bool(true)
