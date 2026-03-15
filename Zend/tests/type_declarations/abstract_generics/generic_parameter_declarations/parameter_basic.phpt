--TEST--
Parameter generic class as type
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

function bar(I<string> $v) {
	var_dump($v);
}

$cs = new CS();
$ci = new CI();

bar($cs);
bar($ci);

?>
--EXPECT--
object(CS)#1 (0) {
}
object(CI)#2 (0) {
}
