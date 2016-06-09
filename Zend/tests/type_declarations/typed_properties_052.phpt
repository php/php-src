--TEST--
Class properties declared in eval() must not leak
--FILE--
<?php

eval(<<<'EOF'
class A {
	public A      $a1;
	public \B     $b1;
	public Foo\C  $c1;
	public ?A     $a2;
	public ?\B    $b2;
	public ?Foo\C $c2;
}
EOF
);
$obj = new A;
var_dump($obj);
?>
--EXPECT--
object(A)#1 (3) {
  ["a1"]=>
  uninitialized(A)
  ["b1"]=>
  uninitialized(B)
  ["c1"]=>
  uninitialized(Foo\C)
  ["a2"]=>
  NULL
  ["b2"]=>
  NULL
  ["c2"]=>
  NULL
}
