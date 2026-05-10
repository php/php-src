--TEST--
Diamond + nullable union: implementer missing null in param is rejected
--FILE--
<?php
interface Setter<in X> { public function set(X $v): void; }
interface Flex extends Setter<?int>, Setter<string> {}

class Bad implements Flex {
    public function set(int|string $v): void {}
}
?>
--EXPECTF--
Fatal error: Declaration of Bad::set(string|int $v): void must be compatible with Setter::set(string|int|null $v): void in %s on line %d
