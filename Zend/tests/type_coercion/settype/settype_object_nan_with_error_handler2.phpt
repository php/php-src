--TEST--
Error handler dtor NAN value, set to object 2
--FILE--
<?php

set_error_handler(function ($errno, $errstr) {
    global $nan;
    unset($nan);
    echo $errstr, "\n";
});

$nan = fdiv(0, 0);
var_dump($nan);
settype($nan, 'object');
var_dump($nan);

?>
--EXPECT--
float(NAN)
unexpected NAN value was coerced to object
object(stdClass)#2 (1) {
  ["scalar"]=>
  float(NAN)
}
