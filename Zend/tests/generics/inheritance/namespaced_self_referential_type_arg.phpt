--TEST--
Inheritance: a namespaced class used as a type argument is namespace-resolved (self-referential)
--FILE--
<?php
namespace G;

interface Eq<T> {
    public function eq(T $o): bool;
}

interface AmountI extends Eq<AmountI> {
    public function eq(AmountI $o): bool;
}

interface Other {}
interface Uses extends Eq<Other> {
    public function eq(Other $o): bool;
}

class Amount implements AmountI {
    public function eq(AmountI $o): bool { return $o === $this; }
}

$a = new Amount();
\var_dump($a->eq($a));
echo "ok\n";
?>
--EXPECT--
bool(true)
ok
