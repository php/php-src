--TEST--
Traits: two classes using the same generic trait can implement the abstract method at distinct substituted types
--FILE--
<?php
trait Thing<T> {
    public abstract function foo(T $v): T;
}

class StrThing {
    use Thing<string>;
    public function foo(string $v): string { return $v; }
}

class IntThing {
    use Thing<int>;
    public function foo(int $v): int { return $v; }
}

echo (new StrThing)->foo("a"), "\n";
echo (new IntThing)->foo(7), "\n";
?>
--EXPECT--
a
7
