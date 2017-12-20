--TEST--
Bug #75644:round(362.42499999999995, 2) gives incorrect result

--FILE--
<?php
$x = 362.42499999999995;
echo $x . "\n";
echo round($x, 2) . "\n";
ini_set('precision', 999);
echo $x . "\n";
echo round($x, 2) . "\n";
?>

--EXPECT--
362.425
362.42
362.424999999999954525264911353588104248046875
362.42000000000001591615728102624416351318359375
