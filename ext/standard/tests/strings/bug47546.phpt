--TEST--
Bug #47546 (Default value for limit parameter in explode is 0, not -1)
--FILE--
<?php
$str = 'one|two|three|four';

print_r(explode('|', $str));
print_r(explode('|', $str, -1));
?>
--EXPECT--
Array
(
    [0] => one
    [1] => two
    [2] => three
    [3] => four
)
Array
(
    [0] => one
    [1] => two
    [2] => three
    [3] => four
)
