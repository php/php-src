--TEST--
Parametric LSP: optional parameter substituted with the bound argument
--FILE--
<?php
interface Maybe<T> {
    public function set(T $value, bool $flag = false): void;
}

class IntMaybe implements Maybe<int> {
    public function set(int $value, bool $flag = false): void { echo "$value/", (int)$flag, "\n"; }
}

$m = new IntMaybe;
$m->set(5);
$m->set(6, true);

try {
    /** @phpstan-ignore-next-line */
    $m->set("x");
} catch (TypeError) {
    echo "TypeError\n";
}
?>
--EXPECT--
5/0
6/1
TypeError
