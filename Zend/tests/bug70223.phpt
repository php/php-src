--TEST--
Bug #70223 (Incrementing value returned by magic getter)
--FILE--
<?php

class A {

    private $foo = 0;

    public function &__get($foo){ return $this->foo; }

}

// Note that this will use __get() for the read, but actually create a new dynamic
// property A::$f for the write.
$a = new A;
echo $a->f++;
echo $a->f++;
echo $a->f++;
?>
--EXPECTF--
Deprecated: Creation of dynamic property A::$f is deprecated in %s on line %d
012
