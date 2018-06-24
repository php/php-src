--TEST--
__compareTo: Inheritance of magic method
--FILE--
<?php
include("Comparable.inc");

/**
 * Returns NULL for all comparisons, which should fall back to the default
 * comparison behaviour, which will take the decoy into account.
 */
class DeferredComparable extends Comparable
{
    public function __compareTo($other) {}
}


/**
 * Inherits base behaviour
 */
class InheritedComparable extends Comparable
{

}

/**
 * Inherits and overrides base behaviour through another.
 */
class ReversedComparable extends InheritedComparable
{
    public function __compareTo($other) {
        return parent::__compareTo($other) * -1;
    }
}

/**
 * Compare against an extending object
 */
var_dump(new Comparable(1, 2) <=> new InheritedComparable(0, 1));
var_dump(new Comparable(1, 2) <=> new InheritedComparable(1, 1));
var_dump(new Comparable(1, 2) <=> new InheritedComparable(2, 1));

var_dump(new InheritedComparable(1, 2) <=> new Comparable(0, 1));
var_dump(new InheritedComparable(1, 2) <=> new Comparable(1, 1));
var_dump(new InheritedComparable(1, 2) <=> new Comparable(2, 1));

/**
 * Compare against an extending object that overrides the base behaviour.
 */
var_dump(new Comparable(1, 2) <=> new ReversedComparable(0, 1));
var_dump(new Comparable(1, 2) <=> new ReversedComparable(1, 1));
var_dump(new Comparable(1, 2) <=> new ReversedComparable(2, 1));

var_dump(new ReversedComparable(1, 2) <=> new Comparable(0, 1)); //  --.
var_dump(new ReversedComparable(1, 2) <=> new Comparable(1, 1)); //    |== These will use ReversedComparable's __compareTo
var_dump(new ReversedComparable(1, 2) <=> new Comparable(2, 1)); //  --'


?>
--EXPECTF--

int(1)
int(0)
int(-1)
int(1)
int(0)
int(-1)
int(1)
int(0)
int(-1)
int(-1)
int(0)
int(1)
