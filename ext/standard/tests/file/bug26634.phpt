--TEST--
Bug #26634 (fgetcsv() incorrect handling of multiline entries with backslashes)
--FILE--
<?php
$fp = fopen(dirname(__FILE__).'/test4.csv', 'r');
while ($row = fgetcsv($fp, 4096, "\t")) {
	print_r($row);
}
?>
--EXPECT--
Array
(
    [0] =>  
    [1] => 
    [2] => 971221
    [3] => 1
)
Array
(
    [0] =>  
    [1] => 
    [2] => 950707
    [3] => \\
\\
)
Array
(
    [0] =>  
    [1] => 
    [2] => 999637
    [3] => ERYC 250
)
