--TEST--
Test pow() with gmp object
--EXTENSIONS--
gmp
--FILE--
<?php

$n = gmp_init(2);
var_dump(pow($n, 10));
var_dump($n ** 10);

try {
    pow($n, -10);
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    $n ** -10;
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}

?>
--EXPECTF--
object(GMP)#%d (1) {
  ["num"]=>
  string(4) "1024"
}
object(GMP)#%d (1) {
  ["num"]=>
  string(4) "1024"
}
Exponent must be greater than or equal to 0
Exponent must be greater than or equal to 0
