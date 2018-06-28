--TEST--
__compareTo: Compare against NULL
--FILE--
<?php

class Comparable
{
    private $value;

    public function __construct($value)
    {
        $this->value = $value;
    }

    public function __compareTo($other)
    {
        echo "Comparing!\n";
        return $this->value <=> $other;
    }
}

/**
 * This should call __compareTo.
 */
var_dump(new Comparable(1) <=> null);

/**
 * This should also call __compareTo even though we're using the RHS
 */
var_dump(null <=> new Comparable(1));

/**
 * We're doing a non-strict comparison between 0 and NULL here.
 */
var_dump(null <=> new Comparable(0));
var_dump(new Comparable(0) <=> null);

echo "\n";

/**
 * Check that default behaviour still works as expected.
 */
var_dump(new stdClass ==  null);
var_dump(new stdClass >=  null);
var_dump(new stdClass <=  null);
var_dump(new stdClass <   null);
var_dump(new stdClass >   null);
var_dump(new stdClass <=> null);

echo "\n";

var_dump(null ==  new stdClass);
var_dump(null >=  new stdClass);
var_dump(null <=  new stdClass);
var_dump(null <   new stdClass);
var_dump(null >   new stdClass);
var_dump(null <=> new stdClass);

?>
--EXPECT--
Comparing!
int(1)
Comparing!
int(-1)
Comparing!
int(0)
Comparing!
int(0)

bool(false)
bool(true)
bool(false)
bool(false)
bool(true)
int(1)

bool(false)
bool(false)
bool(true)
bool(true)
bool(false)
int(-1)
