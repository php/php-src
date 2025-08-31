--TEST--
ghsa-wpj3-hf5j-x4v4 (__Host-/__Secure- cookie bypass due to partial CVE-2022-31629 fix)
--COOKIE--
..Host-test=ignore_1;
._Host-test=ignore_2;
.[Host-test=ignore_3;
_.Host-test=ignore_4;
__Host-test=ignore_5;
_[Host-test=ignore_6;
[.Host-test=ignore_7;
[_Host-test=ignore_8;
[[Host-test=ignore_9;
..Host-test[]=ignore_10;
._Host-test[]=ignore_11;
.[Host-test[]=ignore_12;
_.Host-test[]=ignore_13;
__Host-test[]=legitimate_14;
_[Host-test[]=legitimate_15;
[.Host-test[]=ignore_16;
[_Host-test[]=ignore_17;
[[Host-test[]=ignore_18;
..Secure-test=ignore_1;
._Secure-test=ignore_2;
.[Secure-test=ignore_3;
_.Secure-test=ignore_4;
__Secure-test=ignore_5;
_[Secure-test=ignore_6;
[.Secure-test=ignore_7;
[_Secure-test=ignore_8;
[[Secure-test=ignore_9;
..Secure-test[]=ignore_10;
._Secure-test[]=ignore_11;
.[Secure-test[]=ignore_12;
_.Secure-test[]=ignore_13;
__Secure-test[]=legitimate_14;
_[Secure-test[]=legitimate_15;
[.Secure-test[]=ignore_16;
[_Secure-test[]=ignore_17;
[[Secure-test[]=ignore_18;
--FILE--
<?php
var_dump($_COOKIE);
?>
--EXPECT--
array(3) {
  ["__Host-test"]=>
  array(1) {
    [0]=>
    string(13) "legitimate_14"
  }
  ["_"]=>
  array(2) {
    ["Host-test["]=>
    string(13) "legitimate_15"
    ["Secure-test["]=>
    string(13) "legitimate_15"
  }
  ["__Secure-test"]=>
  array(1) {
    [0]=>
    string(13) "legitimate_14"
  }
}
