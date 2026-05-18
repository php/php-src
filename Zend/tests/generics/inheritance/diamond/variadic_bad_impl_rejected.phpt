--TEST--
Diamond + variadic: bad impl rejected by LSP with correctly substituted error message
--FILE--
<?php
interface Sink<-X> { public function sink(X ...$xs): void; }
class Bad implements Sink<int>, Sink<string> {
    public function sink(int ...$xs): void {}
}
?>
--EXPECTF--
Fatal error: Declaration of Bad::sink(int ...$xs): void must be compatible with Sink::sink(string ...$xs): void in %s on line %d
