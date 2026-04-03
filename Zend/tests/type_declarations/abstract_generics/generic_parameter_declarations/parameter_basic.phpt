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
try {
	bar($ci);
} catch (Throwable $e) {
	echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

?>
--EXPECTF--
object(CS)#1 (0) {
}
TypeError: bar(): Argument #1 ($v) must be of type I<string>, CI given, called in %s on line %d
