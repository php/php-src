--TEST--
Test count() function : countable object with and without modes
--FILE--
<?php
/**
 * Prototype  : int count(array|Countable|null $var [, int $mode])
 * Description: Count the number of elements in a variable (usually an array)
 * Source code: ext/standard/array.c
 */
$countableObject = new class implements Countable {
    public function count()
    {
        return 5;
    }
};

echo "-- Testing count() in default mode --\n";
var_dump( count($countableObject) );
echo "-- Testing count() in COUNT_NORMAL mode --\n";
var_dump( count($countableObject, COUNT_NORMAL) );
echo "-- Testing count() in COUNT_RECURSIVE mode --\n";
var_dump( count($countableObject, COUNT_RECURSIVE) );

?>
--EXPECT--
-- Testing count() in default mode --
int(5)
-- Testing count() in COUNT_NORMAL mode --
int(5)
-- Testing count() in COUNT_RECURSIVE mode --
int(5)
