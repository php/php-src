--TEST--
Bug #69264 (__debugInfo() ignored while extending SPL classes)
--FILE--
<?php
class foo extends SplStack {
    public function __debugInfo() { return ['foo']; }
}

class bar extends foo {
    public function __debugInfo() { return ['bar']; }
}

class bar2 extends SplObjectStorage {
    public function __debugInfo() { return ['bar2']; }
}

var_dump((new bar), (new bar2));
?>
--EXPECTF--
object(bar)#%d (1) {
  [0]=>
  string(3) "bar"
}
object(bar2)#%d (1) {
  [0]=>
  string(4) "bar2"
}
