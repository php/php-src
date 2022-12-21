--TEST--
Compatibility of values of same name trait constants is checked after their constant expressions are evaluated
--ENV--
ENSURE_CONSTANT_IS_DEFINED_AT_RUNTIME=1
--INI--
variables_order=EGPCS
--FILE--
<?php

// Ensure CONSTANT is defined at runtime
if ($_ENV['ENSURE_CONSTANT_IS_DEFINED_AT_RUNTIME']) {
    define('CONSTANT', 2);
}

trait TestTrait {
  public const Constant = 40 + CONSTANT;
}

class ComposingClass {
    use TestTrait;
    public const Constant = 42;
}

echo ComposingClass::Constant;
?>
--EXPECTF--
42
