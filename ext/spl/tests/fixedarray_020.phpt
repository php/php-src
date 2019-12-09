--TEST--
SPL: FixedArray: fromArray/toArray + get_properties
--FILE--
<?php
$a = array(1=>'foo', 2=>'bar', 0=>'gee');
$fa = SplFixedArray::fromArray($a, false);
var_dump(count($fa), $fa->toArray() === array_values($a));

$fa = SplFixedArray::fromArray($a, true);
var_dump(count($fa), $fa->toArray() === $a, $fa->toArray() === (array)$fa);

echo "From Array with string keys, no preserve\n";
SplFixedArray::fromArray(array("foo"=>"bar"), false);
echo "No exception\n";

echo "From Array with string keys, preserve\n";
try {
    SplFixedArray::fromArray(array("foo"=>"bar"), true);
    echo "No exception" . PHP_EOL;
} catch (\ValueError $e) {
    echo $e->getMessage() . PHP_EOL;
}

?>
--EXPECT--
int(3)
bool(true)
int(3)
bool(false)
bool(true)
From Array with string keys, no preserve
No exception
From Array with string keys, preserve
array must contain only positive integer keys
