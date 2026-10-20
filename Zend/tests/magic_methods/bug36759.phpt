--TEST--
Bug #36759 (Objects destructors are invoked in wrong order when script is finished)
--FILE--
<?php
class Foo {
  private $bar;
  function __construct($bar) {
    $this->bar = $bar;
  }
  function __destruct() {
    echo __METHOD__,"\n";
    unset($this->bar);
  }
}

class Bar {
  function __destruct() {
    echo __METHOD__,"\n";
    unset($this->bar);
  }
}
$y = new Bar();
$x = new Foo($y);
?>
--EXPECT--
Foo::__destruct
Bar::__destruct
