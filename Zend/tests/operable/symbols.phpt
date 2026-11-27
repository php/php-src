--TEST--
Operable, OperandSide, and InvalidOperatorError built-in symbols
--FILE--
<?php

var_dump(interface_exists(Operable::class));
var_dump(enum_exists(OperandSide::class));
var_dump(OperandSide::Left, OperandSide::Right);
var_dump(OperandSide::cases());
var_dump(OperandSide::Left instanceof UnitEnum);
var_dump(OperandSide::Left instanceof BackedEnum);
var_dump(class_exists(InvalidOperatorError::class));
var_dump(is_subclass_of(InvalidOperatorError::class, TypeError::class));
var_dump(is_subclass_of(InvalidOperatorError::class, Error::class));
var_dump(is_subclass_of(InvalidOperatorError::class, Throwable::class));

try {
    throw new InvalidOperatorError("unsupported operand");
} catch (TypeError $e) {
    echo get_class($e) . ": " . $e->getMessage() . "\n";
}

?>
--EXPECT--
bool(true)
bool(true)
enum(OperandSide::Left)
enum(OperandSide::Right)
array(2) {
  [0]=>
  enum(OperandSide::Left)
  [1]=>
  enum(OperandSide::Right)
}
bool(true)
bool(false)
bool(true)
bool(true)
bool(true)
bool(true)
InvalidOperatorError: unsupported operand
