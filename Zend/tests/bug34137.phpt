--TEST--
Bug #34137 (assigning array element by reference causes binary mess)
--FILE--
<?php
$arr1 = array('a1' => array('alfa' => 'ok'));
$arr1 =& $arr1['a1'];
echo '-'.$arr1['alfa']."-\n";
?>
--EXPECT--
-ok-
