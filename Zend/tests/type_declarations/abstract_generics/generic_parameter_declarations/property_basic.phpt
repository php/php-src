--TEST--
Property generic class as type
--FILE--
<?php

interface I<T> {
    public function foo(T $param): T;
}

class CS implements I<string> {
    public function foo(string $param): string {
        return $param . '!';
    }
}

class CI implements I<int> {
    public function foo(int $param): int {
        return $param + 42;
    }
}

class T {
	public I<string> $v;
}

$cs = new CS();
$ci = new CI();

$t = new T();
var_dump($t);
$t->v = $cs;
var_dump($t);
$t->v = $ci;
var_dump($t);

?>
--EXPECT--
object(T)#3 (0) {
  ["v"]=>
  uninitialized(I)
}
object(T)#3 (1) {
  ["v"]=>
  object(CS)#1 (0) {
  }
}
object(T)#3 (1) {
  ["v"]=>
  object(CI)#2 (0) {
  }
}
