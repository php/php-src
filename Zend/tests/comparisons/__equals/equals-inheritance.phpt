--TEST--
__equals: Inheritance of magic method
--FILE--
<?php
include("Equatable.inc");

/**
 * Inherits base behaviour
 */
class InheritedEquatable extends Equatable
{

}

/**
 * Inherits and overrides base behaviour through another.
 */
class ReversedEquatable extends InheritedEquatable
{
    public function __equals($other): bool {
        echo "Reverse!\n";

        return ! parent::__equals($other);
    }
}

/**
 * Compare against an extending object
 */
var_dump(new Equatable(1) == new InheritedEquatable(0)); // false
var_dump(new Equatable(1) == new InheritedEquatable(1)); // true
var_dump(new Equatable(1) == new InheritedEquatable(2)); // false

echo "\n";

var_dump(new InheritedEquatable(1) == new Equatable(0)); // false
var_dump(new InheritedEquatable(1) == new Equatable(1)); // true
var_dump(new InheritedEquatable(1) == new Equatable(2)); // false

echo "\n";

/**
 * Compare against an extending object that overrides the base behaviour.
 */
var_dump(new Equatable(1) == new ReversedEquatable(0)); // false
var_dump(new Equatable(1) == new ReversedEquatable(1)); // true, uses Equatable::__equals
var_dump(new Equatable(1) == new ReversedEquatable(2)); // false

echo "\n";

/**
 * These will use ReversedEquatable's __equals
 */
var_dump(new ReversedEquatable(1) == new Equatable(0)); // true
var_dump(new ReversedEquatable(1) == new Equatable(1)); // false 
var_dump(new ReversedEquatable(1) == new Equatable(2)); // true


?>
--EXPECT--
bool(false)
bool(true)
bool(false)

bool(false)
bool(true)
bool(false)

bool(false)
bool(true)
bool(false)

Reverse!
bool(true)
Reverse!
bool(false)
Reverse!
bool(true)
