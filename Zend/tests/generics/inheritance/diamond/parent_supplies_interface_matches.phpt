--TEST--
Diamond: parent's transitive binding matches a direct interface binding
--FILE--
<?php
interface Box<T> {}
class P<T> implements Box<T> {}

class C extends P<int> implements Box<int> {}
echo "OK\n";
?>
--EXPECT--
OK
