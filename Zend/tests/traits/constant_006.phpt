--TEST--
Conflicting constants in composing classes with different values should result in a fatal error, since this indicates that the code is incompatible.
--FILE--
<?php

trait TestTrait {
  public const Constant = 123;
}

echo "PRE-CLASS-GUARD\n";

class ComposingClass {
    use TestTrait;
    public const Constant = 456;
}

echo "POST-CLASS-GUARD\n";
?>
--EXPECTF--
PRE-CLASS-GUARD

Fatal error: ComposingClass and TestTrait define the same constant (Constant) in the composition of ComposingClass. However, the definition differs and is considered incompatible. Class was composed in %s on line %d
