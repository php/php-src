--TEST--
__compareTo: Compare against NULL
--FILE--
<?php

class Comparable
{
    public function __compareTo($other)
    {
        var_dump("Comparing!");
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

?>
--EXPECTF--

string(10) "Comparing!"
int(1)
string(10) "Comparing!"
int(-1)
