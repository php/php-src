--TEST--
__equals: Disallow comparison, but allow equality checks
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
        throw new Exception("This object does not support comparison");
    }

    public function __equals($other)
    {
        return $other instanceof self && $this->value === $other->value;
    }
}


/**
 * 
 */
var_dump(new Comparable(1) == new Comparable(2));

/**
 * 
 */
var_dump(new Comparable(2) == new Comparable(2));

/**
 * 
 */
try {
    new Comparable(2) <= new Comparable(2);
} catch (Exception $e) {
    var_dump($e->getMessage());
}

/**
 * 
 */
try {
    new Comparable(1) > new Comparable(2);
} catch (Exception $e) {
    var_dump($e->getMessage());
}

?>
--EXPECTF--
bool(false)
bool(true)
string(39) "This object does not support comparison"
string(39) "This object does not support comparison"
