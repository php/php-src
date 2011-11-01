--TEST--
Bug #60153 (Interface method prototypes not enforced when implementd via traits.)
--FILE--
<?php

interface IFoo {
    public function oneArgument($a);
}

trait TFoo {
  public function oneArgument() {}
}

class C implements IFoo {
  use TFoo;
}

--EXPECTF--
Fatal error: Declaration of C::oneArgument() must be compatible with IFoo::oneArgument($a) in %s on line %d
