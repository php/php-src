--TEST--
Testing array dereference with dynamic method name and references
--FILE--
<?php

error_reporting(E_ALL);

class foo {
    public $x = array(1);

    public function &b() {
        return $this->x;
    }
}

$foo = new foo;

$a = 'b';
var_dump($foo->$a()[0]);

$h = &$foo->$a();
$h[] = 2;
var_dump($foo->$a());

?>
--EXPECT--
int(1)
array(2) {
  [0]=>
  int(1)
  [1]=>
  int(2)
}
