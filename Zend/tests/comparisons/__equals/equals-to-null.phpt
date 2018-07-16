--TEST--
__equals: Compare against NULL
--FILE--
<?php

class Equatable
{
    private $value;

    public function __construct($value)
    {
        $this->value = $value;
    }

    public function __equals($other)
    {
        echo "Comparing!\n";
        return $this->value == $other;
    }
}

/**
 * This should call __equals.
 */
var_dump(new Equatable(1) == null); // false

/**
 * This should also call __equals even though we're using the RHS
 */
var_dump(null == new Equatable(1)); // false

/**
 * We're doing a non-strict comparison between 0 and NULL here.
 */
var_dump(null == new Equatable(0)); // true
var_dump(new Equatable(0) == null); // true

echo "\n";

/**
 * Check that default behaviour still works as expected.
 */
var_dump(new stdClass == null);
var_dump(new stdClass != null);
var_dump(null == new stdClass);
var_dump(null != new stdClass);

?>
--EXPECT--
Comparing!
bool(false)
Comparing!
bool(false)
Comparing!
bool(true)
Comparing!
bool(true)

bool(false)
bool(true)
bool(false)
bool(true)
