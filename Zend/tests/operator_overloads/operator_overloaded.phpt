--TEST--
operator overload: overload called
--FILE--
<?php

class A {
    public function __add(mixed $other, bool $left): self
    {
    	echo "__add() called\n";
    	return $this;
    }
    public function __sub(mixed $other, bool $left): self
    {
    	echo "__sub() called\n";
    	return $this;
    }
    public function __mul(mixed $other, bool $left): self
    {
    	echo "__mul() called\n";
    	return $this;
    }
    public function __div(mixed $other, bool $left): self
    {
    	echo "__div() called\n";
    	return $this;
    }
    public function __mod(mixed $other, bool $left): self
    {
    	echo "__mod() called\n";
    	return $this;
    }
    public function __pow(mixed $other, bool $left): self
    {
    	echo "__pow() called\n";
    	return $this;
    }
    public function __bitwiseAnd(mixed $other, bool $left): self
    {
    	echo "__bitwiseAnd() called\n";
    	return $this;
    }
    public function __bitwiseOr(mixed $other, bool $left): self
    {
    	echo "__bitwiseOr() called\n";
    	return $this;
    }
    public function __bitwiseXor(mixed $other, bool $left): self
    {
    	echo "__bitwiseXor() called\n";
    	return $this;
    }
    public function __bitwiseNot(): self
    {
    	echo "__bitwiseNot() called\n";
    	return $this;
    }
    public function __bitwiseShiftLeft(mixed $other, bool $left): self
    {
    	echo "__bitwiseShiftLeft() called\n";
    	return $this;
    }
    public function __bitwiseShiftRight(mixed $other, bool $left): self
    {
    	echo "__bitwiseShiftRight() called\n";
    	return $this;
    }
}

$obj = new A();

$obj + 1;
1 + $obj;
$obj - 1;
1 - $obj;
$obj * 1;
1 * $obj;
$obj / 1;
1 / $obj;
$obj % 1;
1 % $obj;
$obj ** 1;
1 ** $obj;
$obj & 1;
1 & $obj;
$obj | 1;
1 | $obj;
$obj ^ 1;
1 ^ $obj;
$obj << 1;
1 << $obj;
$obj >> 1;
1 >> $obj;
~$obj;

?>
--EXPECT--
__add() called
__add() called
__sub() called
__sub() called
__mul() called
__mul() called
__div() called
__div() called
__mod() called
__mod() called
__pow() called
__pow() called
__bitwiseAnd() called
__bitwiseAnd() called
__bitwiseOr() called
__bitwiseOr() called
__bitwiseXor() called
__bitwiseXor() called
__bitwiseShiftLeft() called
__bitwiseShiftLeft() called
__bitwiseShiftRight() called
__bitwiseShiftRight() called
__bitwiseNot() called
