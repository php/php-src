<?php

/**
 * This is a class that compares by a given $value only. There is also a $decoy
 * value that makes sure that the tests would fail if `__compareTo` is not used.
 */
class Comparable
{
    protected $decoy;
    protected $value;
    
    public function __construct($value) {
        $this->decoy = $value * -1;
        $this->value = $value;
    }  

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

        throw new Exception("Failed to compare");
    }
}
