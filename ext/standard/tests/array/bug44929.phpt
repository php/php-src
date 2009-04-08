--TEST--
Bug #44929 (natsort doesn't handle leading zeros well)
--FILE--
<?php
$a = array('001','008','005','00011','03','000014','-123','0.002','00','0');
natsort($a);
var_dump($a);
?>
--EXPECT--
array(10) {
  [6]=>
  string(4) "-123"
  [7]=>
  string(5) "0.002"
  [8]=>
  string(2) "00"
  [9]=>
  string(1) "0"
  [0]=>
  string(3) "001"
  [4]=>
  string(2) "03"
  [2]=>
  string(3) "005"
  [1]=>
  string(3) "008"
  [3]=>
  string(5) "00011"
  [5]=>
  string(6) "000014"
}
