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
Fatal error: The modifiers for the trait alias dontKnow() need to be changed in the same statment in which the alias is defined. Error in %s on line %d
