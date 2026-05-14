--TEST--
BackedEnum::values() returns array of backing values
--FILE--
<?php

enum Status: string {
    case ACTIVE = 'active';
    case INACTIVE = 'inactive';
}

enum HttpStatus: int {
    case OK = 200;
    case NOT_FOUND = 404;
    case SERVER_ERROR = 500;
}

enum UnitOnly {
    case A;
    case B;
}

// Test string-backed enum
echo "String-backed:\n";
var_dump(Status::values());

// Test int-backed enum
echo "Int-backed:\n";
var_dump(HttpStatus::values());

// Verify it replaces boilerplate array_column(Status::cases(), 'value')
echo "Equivalence with array_column:\n";
var_dump(Status::values() === array_column(Status::cases(), 'value'));
var_dump(HttpStatus::values() === array_column(HttpStatus::cases(), 'value'));

// Verify UnitEnum doesn't have values() method
echo "UnitEnum error:\n";
try {
    UnitOnly::values();
} catch (Error $e) {
    echo $e->getMessage() . "\n";
}

?>
--EXPECTF--
String-backed:
array(2) {
  [0]=>
  string(6) "active"
  [1]=>
  string(8) "inactive"
}
Int-backed:
array(3) {
  [0]=>
  int(200)
  [1]=>
  int(404)
  [2]=>
  int(500)
}
Equivalence with array_column:
bool(true)
bool(true)
UnitEnum error:
Call to undefined method UnitOnly::values()