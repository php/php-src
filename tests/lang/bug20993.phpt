--TEST--
Bug #20993 (referenced array key, makes array global)
--FILE--
<?php
$a = array(1);
$b = array(1);
$ref =& $a[0];

echo $a[0], ' -b ', $b[0], ' -r ', $ref,  "\n";
changeVal($a);
echo $a[0], ' -b ', $b[0], ' -r ', $ref,  "\n";
changeVal($b);
echo $a[0], ' -b ', $b[0], ' -r ', $ref,  "\n";

function changeVal($arr)
{
	$arr[0] = 2;
}
?>
--EXPECT--
1 -b 1 -r 1
1 -b 1 -r 1
1 -b 1 -r 1
