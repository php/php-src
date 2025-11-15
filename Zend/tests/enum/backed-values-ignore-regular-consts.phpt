--TEST--
BackedEnum::values() only returns case values, not regular constants
--FILE--
<?php
enum Mixed_: string {
    case A = 'a';
    case B = 'b';

    public const REGULAR_CONST = 'not_a_case';
}
var_dump(Mixed_::values());
?>
--EXPECT--
array(2) {
  [0]=>
  string(1) "a"
  [1]=>
  string(1) "b"
}

