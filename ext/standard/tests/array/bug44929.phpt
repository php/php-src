--TEST--
Bug #44929 (natsort doesn't handle leading zeros well)
--FILE--
<?php
$a = array(b'001',b'008',b'005',b'00011',b'03',b'000014',b'-123',b'0.002',b'00',b'0',b'0_0',b'0-0');
natsort($a);
var_dump($a);
?>
--EXPECT--
array(12) {
  [6]=>
  string(4) "-123"
  [8]=>
  string(2) "00"
  [9]=>
  string(1) "0"
  [11]=>
  string(3) "0-0"
  [7]=>
  string(5) "0.002"
  [10]=>
  string(3) "0_0"
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
