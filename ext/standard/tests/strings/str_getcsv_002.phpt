--TEST--
str_getcsv() with empty $escape
--FILE--
<?php
$contents = <<<EOS
"cell1","cell2\\","cell3","cell4"
EOS;
print_r(str_getcsv($contents, ',', '"', ''));
?>
--EXPECT--
Array
(
    [0] => cell1
    [1] => cell2\
    [2] => cell3
    [3] => cell4
)
