--TEST--
Bug #70223 (Incrementing value returned by magic getter)
--FILE--
<?php

// Note that this actually writes to dynamic property A::$f.
// Increment goes through __set(), not __get() by reference!
#[AllowDynamicProperties]
class A {

    private $foo = 0;

    public function &__get($foo){ return $this->foo; }

}

$a = new A;
echo $a->f++;
echo $a->f++;
echo $a->f++;
?>
--EXPECT--
012
