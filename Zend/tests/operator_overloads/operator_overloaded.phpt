--TEST--
operator overload: overload called
--FILE--
<?php

class A {
    public operator +(mixed $other, OperandPosition $left): self
    {
    	echo "+() called\n";
    	return $this;
    }
    public operator -(mixed $other, OperandPosition $left): self
    {
    	echo "-() called\n";
    	return $this;
    }
    public operator *(mixed $other, OperandPosition $left): self
    {
    	echo "*() called\n";
    	return $this;
    }
    public operator /(mixed $other, OperandPosition $left): self
    {
    	echo "/() called\n";
    	return $this;
    }
    public operator %(mixed $other, OperandPosition $left): self
    {
    	echo "%() called\n";
    	return $this;
    }
    public operator **(mixed $other, OperandPosition $left): self
    {
    	echo "**() called\n";
    	return $this;
    }
    public operator &(mixed $other, OperandPosition $left): self
    {
    	echo "&() called\n";
    	return $this;
    }
    public operator |(mixed $other, OperandPosition $left): self
    {
    	echo "|() called\n";
    	return $this;
    }
    public operator ^(mixed $other, OperandPosition $left): self
    {
    	echo "^() called\n";
    	return $this;
    }
    public operator ~(): self
    {
    	echo "~() called\n";
    	return $this;
    }
    public operator <<(mixed $other, OperandPosition $left): self
    {
    	echo "<<() called\n";
    	return $this;
    }
    public operator >>(mixed $other, OperandPosition $left): self
    {
    	echo ">>() called\n";
    	return $this;
    }
}

$obj = new A();

$obj + 1;
1 + $obj;
$obj = $obj + 1;
$obj += 1;
$obj++;
++$obj;
$obj - 1;
1 - $obj;
$obj = $obj - 1;
$obj -= 1;
$obj--;
--$obj;
$obj * 1;
1 * $obj;
$obj = $obj * 1;
$obj *= 1;
-$obj;
$obj / 1;
1 / $obj;
$obj = $obj / 1;
$obj /= 1;
$obj % 1;
1 % $obj;
$obj = $obj % 1;
$obj %= 1;
$obj ** 1;
1 ** $obj;
$obj = $obj ** 1;
$obj **= 1;
$obj & 1;
1 & $obj;
$obj = $obj & 1;
$obj &= 1;
$obj | 1;
1 | $obj;
$obj = $obj | 1;
$obj |= 1;
$obj ^ 1;
1 ^ $obj;
$obj = $obj ^ 1;
$obj ^= 1;
$obj << 1;
1 << $obj;
$obj = $obj << 1;
$obj <<= 1;
$obj >> 1;
1 >> $obj;
$obj = $obj >> 1;
$obj >>= 1;
~$obj;

?>
--EXPECT--
+() called
+() called
+() called
+() called
+() called
+() called
-() called
-() called
-() called
-() called
-() called
-() called
*() called
*() called
*() called
*() called
*() called
/() called
/() called
/() called
/() called
%() called
%() called
%() called
%() called
**() called
**() called
**() called
**() called
&() called
&() called
&() called
&() called
|() called
|() called
|() called
|() called
^() called
^() called
^() called
^() called
<<() called
<<() called
<<() called
<<() called
>>() called
>>() called
>>() called
>>() called
~() called
