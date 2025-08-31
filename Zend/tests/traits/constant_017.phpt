--TEST--
Referencing trait constants directly on calling \defined() returns false
--FILE--
<?php

trait TestTrait {
  public const Constant = 42;
}

var_dump(\defined('TestTrait::Constant'));
?>
--EXPECTF--
bool(false)
