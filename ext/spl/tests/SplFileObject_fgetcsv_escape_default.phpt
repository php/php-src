--TEST--
SplFileObject::fgetcsv with default escape character
--FILE--
<?php
$fp = fopen('SplFileObject__fgetcsv7.csv', 'w+');
fwrite($fp, '"aa\"","bb","\"c"');
fclose($fp);

$fo = new SplFileObject('SplFileObject__fgetcsv7.csv');
var_dump($fo->fgetcsv());
?>
--CLEAN--
<?php
unlink('SplFileObject__fgetcsv7.csv');
?>
--EXPECT--
array(3) {
  [0]=>
  string(4) "aa\""
  [1]=>
  string(2) "bb"
  [2]=>
  string(3) "\"c"
}
