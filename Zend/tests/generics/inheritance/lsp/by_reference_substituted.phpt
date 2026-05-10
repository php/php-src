--TEST--
Parametric LSP: by-reference parameter substituted with the bound argument
--FILE--
<?php
interface Filler<T> {
    public function fill(T &$slot): void;
}

class IntFiller implements Filler<int> {
    public function fill(int &$slot): void { $slot = 99; }
}

$x = 0;
(new IntFiller)->fill($x);
echo $x, "\n";
?>
--EXPECT--
99
