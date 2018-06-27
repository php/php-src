<?php

/**
 * Base class that has a $value, and $decoy which exists to make sure that we
 * don't get false positives when default behaviour compares $value and our
 * magic methods aren't called at all. 
 */
class Base 
{
    protected $decoy;
    protected $value;
    
    public function __construct($value) {
        $this->decoy = $value * -1;
        $this->value = $value;
    }  
}

/**
 * This is a class that compares by a given $value only. There is also a $decoy
 * value that makes sure that the tests would fail if `__equals` is not used.
 */
class Equatable extends Base
{
    public function __equals($other): bool {
        return $other instanceof self && $this->value === $other->value;
    }
}

/**
 * This is a class that compares by a given $value only. There is also a $decoy
 * value that makes sure that the tests would fail if `__compareTo` is not used.
 */
class Comparable extends Base
{
    public function __compareTo($other) {
        if ($other instanceof self) {
            return $this->value <=> $other->value;
        }

        /**
         * Allow comparing to scalar numeric values.
         */
        if (is_numeric($other)) {
            return $this->value <=> $other;
        }
    }
}
