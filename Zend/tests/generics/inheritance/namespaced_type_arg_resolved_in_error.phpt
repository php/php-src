--TEST--
Inheritance: namespaced type argument is fully resolved in incompatibility errors
--FILE--
<?php
namespace G;

interface Eq<T> {
    public function eq(T $o): bool;
}

interface AmountI extends Eq<AmountI> {
    public function eq(int $o): bool;
}
?>
--EXPECTF--
Fatal error: Declaration of G\AmountI::eq(int $o): bool must be compatible with G\Eq::eq(G\AmountI $o): bool in %s on line %d
