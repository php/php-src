--TEST--
Bug #68128 - RecursiveRegexIterator raises "Array to string conversion" notice
--FILE--
<?php

$array = new ArrayIterator(array('a', array('b', 'c')));
$regex = new RegexIterator($array, '/Array/');

foreach ($regex as $match) {
    var_dump($match);
}

$rArrayIterator = new RecursiveArrayIterator(array('test1', array('tet3', 'test4', 'test5')));
$rRegexIterator = new RecursiveRegexIterator($rArrayIterator, '/^(t)est(\d*)/',
    RecursiveRegexIterator::ALL_MATCHES, 0, PREG_PATTERN_ORDER);

foreach ($rRegexIterator as $key1 => $value1) {

    if ($rRegexIterator->hasChildren()) {

        // print all children
        echo "Children: ";
        foreach ($rRegexIterator->getChildren() as $key => $value) {
			print_r($value);
        }
        echo "\n";
    } else {
        echo "No children ";
		print_r($value1);
		echo "\n";
    }
}

?>
--EXPECT--
No children Array
(
    [0] => Array
        (
            [0] => test1
        )

    [1] => Array
        (
            [0] => t
        )

    [2] => Array
        (
            [0] => 1
        )

)

Children: Array
(
    [0] => Array
        (
            [0] => test4
        )

    [1] => Array
        (
            [0] => t
        )

    [2] => Array
        (
            [0] => 4
        )

)
Array
(
    [0] => Array
        (
            [0] => test5
        )

    [1] => Array
        (
            [0] => t
        )

    [2] => Array
        (
            [0] => 5
        )

)

