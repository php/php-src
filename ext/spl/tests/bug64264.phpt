--TEST--
Bug #64264 (SPLFixedArray toArray problem)
--FILE--
<?php
class MyFixedArray extends \SplFixedArray {                                                                                                                                 
    protected $foo;                                                                                                                                                         
    protected $bar;                                                                                                                                                         
}                                                                                                                                                                           

$myFixedArr = new MyFixedArray(1);                                                                                                                                          
$myFixedArr[0] = 'foo';
$myFixedArr->setSize(2);
$myFixedArr[1] = 'bar';
$myFixedArr->setSize(5);
$array = $myFixedArr->toArray();
$array[2] = "ERROR";
$array[3] = "ERROR";
$array[4] = "ERROR";
unset($array[4]);
$myFixedArr->setSize(2);

print_r($myFixedArr->toArray());
?>
--EXPECTF--
Array
(
    [0] => foo
    [1] => bar
)
