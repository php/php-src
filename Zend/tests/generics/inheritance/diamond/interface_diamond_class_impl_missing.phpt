--TEST--
Diamond + variance: class implementing an interface diamond must satisfy the merged signature
--FILE--
<?php
interface Setter<-X> { public function set(X $v): void; }
interface Flex extends Setter<int>, Setter<string> {}

class Bad implements Flex {
    public function set(int $v): void {}
}
?>
--EXPECTF--
Fatal error: Declaration of Bad::set(int $v): void must be compatible with %s::set(string|int $v): void in %s on line %d
