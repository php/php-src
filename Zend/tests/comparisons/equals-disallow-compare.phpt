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
 * This should call __equals which returns FALSE.
 */
var_dump(new Comparable(1) == new Comparable(2));

/**
 * This should call __equals which returns TRUE.
 */
var_dump(new Comparable(2) == new Comparable(2));

/**
 * These should call __compareTo which is explicitly unsupported.
 */
try {
    new Comparable(2) <= new Comparable(2);
} catch (Exception $e) {
    var_dump($e->getMessage());
}

try {
    new Comparable(1) > new Comparable(1);
} catch (Exception $e) {
    var_dump($e->getMessage());
}

try {
    new Comparable(1) < new Comparable(1);
} catch (Exception $e) {
    var_dump($e->getMessage());
}

try {
    new Comparable(1) >= new Comparable(1);
} catch (Exception $e) {
    var_dump($e->getMessage());
}

try {
    new Comparable(1) <= new Comparable(1);
} catch (Exception $e) {
    var_dump($e->getMessage());
}

try {
    new Comparable(1) <=> new Comparable(1);
} catch (Exception $e) {
    var_dump($e->getMessage());
}

?>
--EXPECT--
bool(false)
bool(true)
string(39) "This object does not support comparison"
string(39) "This object does not support comparison"
string(39) "This object does not support comparison"
string(39) "This object does not support comparison"
string(39) "This object does not support comparison"
string(39) "This object does not support comparison"
