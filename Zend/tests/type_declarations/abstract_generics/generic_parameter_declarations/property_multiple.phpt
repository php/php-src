--TEST--
Property generic class as type
--FILE--
<?php

interface I<T1, T2> {
    public function foo(T1 $param): T2;
}

class CS implements I<string, string> {
    public function foo(string $param): string {
        return $param . '!';
    }
}

class CI implements I<int, int> {
    public function foo(int $param): int {
        return $param + 42;
    }
}

class T {
	public I<string, string> $v;
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
  uninitialized(I<string, string>)
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
