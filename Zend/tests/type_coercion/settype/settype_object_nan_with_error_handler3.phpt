--TEST--
Error handler dtor NAN value, set to object 3
--FILE--
<?php

set_error_handler(function ($errno, $errstr) {
    global $nan;
    $nan = bin2hex(random_bytes(4));
    echo $errstr, "\n";
});

$nan = fdiv(0, 0);
var_dump($nan);
settype($nan, 'object');
var_dump($nan);

?>
--EXPECTF--
float(NAN)
unexpected NAN value was coerced to object
object(stdClass)#2 (1) {
  ["scalar"]=>
  string(8) "%s"
}
