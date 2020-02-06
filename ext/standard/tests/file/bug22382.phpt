--TEST--
Bug #22382 (fgetcsv() does not handle escaped quotes correctly)
--FILE--
<?php
$fp = fopen(__DIR__."/test2.csv", "r");
while(($line = fgetcsv($fp, 1024))) {
    var_dump($line);
}
fclose($fp);
?>
--EXPECT--
array(6) {
  [0]=>
  string(3) "One"
  [1]=>
  string(7) "\"Two\""
  [2]=>
  string(7) "Three\""
  [3]=>
  string(4) "Four"
  [4]=>
  string(2) "\\"
  [5]=>
  string(28) "\\\\\\\\\\\\\\\\\\\\\\\"\\\\"
}
