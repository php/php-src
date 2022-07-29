--TEST--
Use of @ before calling generator in foreach throws
--XFAIL--
Not sure what the correct behaviour is meant to be
--FILE--
<?php

function generator() {
    yield 1;
    throw new Exception();
    yield 2;
}

foreach (@generator() as $val) {
    var_dump($val);
}

echo "Done\n";
?>
--EXPECT--
int(1)

Fatal error: Uncaught Exception in /home/girgias/Dev/php-src/Zend/tests/silence_operator/yield.php:5
Stack trace:
#0 /home/girgias/Dev/php-src/Zend/tests/silence_operator/yield.php(9): generator()
#1 {main}
  thrown in /home/girgias/Dev/php-src/Zend/tests/silence_operator/yield.php on line 5
