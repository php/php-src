--TEST--
Error handler dtor NAN value, set to array 3
--FILE--
<?php

set_error_handler(function ($errno, $errstr) {
    global $nan;
    $nan = bin2hex(random_bytes(4));
    echo $errstr, "\n";
});

$nan = fdiv(0, 0);
var_dump($nan);
settype($nan, 'array');
var_dump($nan);

?>
--EXPECTF--
float(NAN)
unexpected NAN value was coerced to array
array(1) {
  [0]=>
  string(8) "%s"
}
