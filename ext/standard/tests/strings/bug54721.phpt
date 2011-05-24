--TEST--
Bug #54721 (Different Hashes on Windows, BSD and Linux on wrong Salt size)
--FILE--
<?php 
echo crypt("", '$1$dW0.is5.$10CH101gGOr1677ZYd517.') . "\n";
echo crypt("b", '$1$dW0.is5.$10CH101gGOr1677ZYd517.') . "\n";
echo crypt("bu", '$1$dW0.is5.$10CH101gGOr1677ZYd517.') . "\n";
echo crypt("bug", '$1$dW0.is5.$10CH101gGOr1677ZYd517.') . "\n";
echo crypt("pass", '$1$dW0.is5.$10CH101gGOr1677ZYd517.') . "\n";
echo crypt("buged", '$1$dW0.is5.$10CH101gGOr1677ZYd517.') . "\n";
echo crypt("aaaaaaaaaaaaaaaaaaaaaaaaa ", '$1$dW0.is5.$10CH101gGOr1677ZYd517.') . "\n";
?>
--EXPECT--
$1$dW0.is5.$I0iqTYHPzkP4YnRgnXxZW0
$1$dW0.is5.$KaspRpPQ9U7Xb5Vv5c.WE/
$1$dW0.is5.$X9G1x/Ep8zYQSrU4/lKUg.
$1$dW0.is5.$wE5Rz/HxPtDMfqil6kK980
$1$dW0.is5.$2E4/ZDY1vr73HqLl1bLs9.
$1$dW0.is5.$lvGhphTQwqgKxWhWwYERr1
$1$dW0.is5.$XzsWcLSBj2BvhOKH0xdpZ0
