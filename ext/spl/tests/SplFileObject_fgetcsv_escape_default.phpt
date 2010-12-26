--TEST--
SplFileObject::fgetcsv with default escape character
--FILE--
<?php
$fp = fopen('SplFileObject__fgetcsv.csv', 'w+');
fwrite($fp, '"aa\"","bb","\"c"');
fclose($fp);

$fo = new SplFileObject('SplFileObject__fgetcsv.csv');
var_dump($fo->fgetcsv());
?>
--CLEAN--
<?php
unlink('SplFileObject__fgetcsv.csv');
?>
--EXPECTF--
array(3) {
  [0]=>
  string(4) "aa\""
  [1]=>
  string(2) "bb"
  [2]=>
  string(3) "\"c"
}
