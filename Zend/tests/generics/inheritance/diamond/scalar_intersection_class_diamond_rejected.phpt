--TEST--
Diamond + invariant T: a class double-implementing with scalar bindings is caught by per-path LSP
--FILE--
<?php
interface Box<T> {
    public function get(): T;
}
interface IntBox extends Box<int> {}
interface StrBox extends Box<string> {}

class C implements IntBox, StrBox {
    public function get() { return 0; }
}
?>
--EXPECTF--
Fatal error: Declaration of C::get() must be compatible with Box::get(): int in %s on line %d
