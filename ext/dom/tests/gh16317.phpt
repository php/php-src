--TEST--
GH-16317 (DOM classes do not allow __debugInfo() overrides to work)
--FILE--
<?php

class Demo extends DOMNode {
    public function __construct() {}
    public function __debugInfo(): array {
        return ['x' => 'y'];
    }
}

var_dump(new Demo());

?>
--EXPECT--
object(Demo)#1 (1) {
  ["x"]=>
  string(1) "y"
}
