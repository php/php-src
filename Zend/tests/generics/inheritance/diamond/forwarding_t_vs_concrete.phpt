--TEST--
Diamond: a generic class forwarding T conflicts with a concrete arg on another path (accepted for the class form, LSP catches actual signature mismatches)
--FILE--
<?php
interface Box<T> {}
interface Wrapper<U> extends Box<U> {}

class C<T> implements Wrapper<T>, Box<int> {}
$c = new C();
var_dump($c instanceof Box, $c instanceof Wrapper);
?>
--EXPECT--
bool(true)
bool(true)
