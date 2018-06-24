<?php

/**
 * This is a class that compares by a given $value only. There is also a $decoy
 * value that makes sure that the tests would fail if `__compareTo` is not used.
 */
class Comparable
{
    protected $value;
    protected $decoy;

    public function __construct($value, $decoy = null) {
        $this->value = $value;
        $this->decoy = $decoy;
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

        /**
         * Return NULL to fall back to default behaviour. This indicates that
         * it's an unsupported comparison and most implementations would throw
         * an exception here to catch unintended behaviour.
         */
        return null;
    }
}
