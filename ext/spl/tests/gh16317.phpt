--TEST--
GH-16317 (__debugInfo() overrides don't work for SplFixedArray subclasses)
--FILE--
<?php

class Demo extends SplFixedArray {
    public function __construct() {}
    public function __debugInfo(): array {
        return ['x' => 'y'];
    }
}
var_dump( new Demo() );

?>
--EXPECT--
object(Demo)#1 (1) {
  ["x"]=>
  string(1) "y"
}
