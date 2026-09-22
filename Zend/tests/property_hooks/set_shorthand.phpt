--TEST--
set shorthand
--FILE--
<?php

class Test {
    public string $prop {
        set => strtoupper($value);
    }
}

$test = new Test();
$test->prop = 'foo';
var_dump($test);

?>
--EXPECT--
object(Test)#1 (1) {
  ["prop"]=>
  string(3) "FOO"
}
