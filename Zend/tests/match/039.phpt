--TEST--
Test match with duplicate conditions
--FILE--
<?php

$value = 1;

echo match ($value) {
    1 => 1,
    2 => 2,
    3 => 3,
    4 => 4,
    5 => 5,
    1 => 6,
};
echo "\n";

echo match ($value) {
    2, 1 => '2, 1',
    1 => 1,
    3 => 3,
    4 => 4,
    5 => 5,
};
echo "\n";

echo match ($value) {
    1, 1 => '1, 1',
    2, 2 => '2, 2',
    3, 3 => '3, 3',
    4, 4 => '4, 4',
    5, 5 => '5, 5',
};
echo "\n";

echo match ($value) {
    1 => 1,
    1 => 2,
};
echo "\n";

echo match ($value) {
    2, 1 => '2, 1',
    1 => 1,
};
echo "\n";

echo match ($value) {
    1, 1 => '1, 1',
    1 => 1,
};
echo "\n";

?>
--EXPECT--
1
2, 1
1, 1
1
2, 1
1, 1
