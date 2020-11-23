--TEST--
Bug #54441 (Changing trait static method visibility)
--FILE--
<?php

trait Foo {
  public function bar() {}
}

class Boo {
  use Foo {
    bar as dontKnow;
    dontKnow as protected;
  }
}

?>
--EXPECTF--
Fatal error: The modifiers of the trait method dontKnow() are changed, but this method does not exist. Error in %s on line %d
