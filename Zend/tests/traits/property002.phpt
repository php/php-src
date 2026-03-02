--TEST--
Non-conflicting properties should work just fine.
--FILE--
<?php

trait THello1 {
  public $hello = "hello";
}

trait THello2 {
  private $world = "World!";
}

class TraitsTest {
    use THello1;
    use THello2;
    function test() {
        echo $this->hello . ' ' . $this->world;
    }
}

var_dump(property_exists('TraitsTest', 'hello'));
var_dump(property_exists('TraitsTest', 'world'));

$t = new TraitsTest;
$t->test();
?>
--EXPECT--
bool(true)
bool(true)
hello World!
