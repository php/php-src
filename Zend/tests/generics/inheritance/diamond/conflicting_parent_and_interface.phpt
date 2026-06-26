--TEST--
Diamond: parent and direct interface providing different bindings for the same invariant ancestor (class case is deferred to per-path LSP)
--FILE--
<?php
interface Box<T> {}
class P implements Box<int> {}

class C extends P implements Box<string> {}
$c = new C();
var_dump($c instanceof Box, $c instanceof P);
?>
--EXPECT--
bool(true)
bool(true)
