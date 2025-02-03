--TEST--
Short method function declaration
--FILE--
<?php
class Describer {
  function getType(): string = $this->type;
  function getTypeName(): string = match($type) {
    "variable" => "Variable",
    "function_return" => "Function Return Type",
    default => "unknown"
  };
}
?>
--EXPECT--
