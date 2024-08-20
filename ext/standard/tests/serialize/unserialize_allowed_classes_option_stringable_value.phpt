--TEST--
Test unserialize() with Stringable object in allowed_classes
--FILE--
<?php
class foo {
        public $x = "bar";
}
$z = array(new foo(), 2, "3");
$s = serialize($z);

class Name {
    public function __toString(): string {
        return 'Foo';
    }
}

$o = new Name();

var_dump(unserialize($s, ["allowed_classes" => [$o]]));
?>
--EXPECT--
array(3) {
  [0]=>
  object(foo)#3 (1) {
    ["x"]=>
    string(3) "bar"
  }
  [1]=>
  int(2)
  [2]=>
  string(1) "3"
}
