--TEST--
operator overload: no explicit type
--FILE--
<?php

class A {
    public int $value;

    public function __add($other, bool $left): self
    {
    	$return = new A();
    	$return->value = $this->value + $other;

		return $return;
    }
}
?>
--EXPECTF--
Fatal error: A::__add(): Parameter #1 ($other) must explicitly define a type in %s on line %d
