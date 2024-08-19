--TEST--
GH-15456: Crash in get_class_vars() on virtual properties
--FILE--
<?php

class C {
    public $a = 42;
    public $b = 42 { get => $this->b * 2; }
    public $c { get => 42; }
}
var_dump(get_class_vars(C::class));

?>
--EXPECT--
array(2) {
  ["a"]=>
  int(42)
  ["b"]=>
  int(42)
}
