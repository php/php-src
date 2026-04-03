--TEST--
Return generic class as type
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

function bar($v): I<string> {
	return $v;
}

$cs = new CS();
$ci = new CI();

var_dump(bar($cs));
try {
	var_dump(bar($ci));
} catch (Throwable $e) {
	echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
object(CS)#1 (0) {
}
TypeError: bar(): Return value must be of type I<string>, CI returned
