--TEST--
Parametric LSP: a class extending a generic class while implementing an unrelated generic interface
--FILE--
<?php
class BaseList<T> {
    public function append(T $value): void { echo "append:$value\n"; }
}

interface Counter<U> {
    public function count(): U;
}

class IntListWithCount extends BaseList<int> implements Counter<int> {
    public function count(): int { return 0; }
}

$l = new IntListWithCount;
$l->append(7);
echo $l->count(), "\n";
?>
--EXPECT--
append:7
0
