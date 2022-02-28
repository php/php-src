--TEST--
Use-after-free when resume an already running generator
--FILE--
<?php
function gen(){
    $g = yield;
    $g->send($y);
}
$gen=gen();
try {
    $gen->send($gen);
}catch(y) {
}
?>
--EXPECTF--
Warning: Undefined variable $y in %sresume_running_generator_error_003.php on line 4

Fatal error: Uncaught Error: Cannot resume an already running generator in %sresume_running_generator_error_003.php:4
Stack trace:
#0 %sresume_running_generator_error_003.php(4): Generator->send(NULL)
#1 [internal function]: gen()
#2 %sresume_running_generator_error_003.php(8): Generator->send(Object(Generator))
#3 {main}
  thrown in %sresume_running_generator_error_003.php on line 4
