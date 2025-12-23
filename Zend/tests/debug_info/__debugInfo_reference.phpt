--TEST--
__debugInfo with reference return
--FILE--
<?php

class Test {
    private $tmp = ['x' => 1];

    public function &__debugInfo(): array
    {
        return $this->tmp;
    }
}

var_dump(new Test);

?>
--EXPECT--
object(Test)#1 (1) {
  ["x"]=>
  int(1)
}
