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
--EXPECTF--
Deprecated: SplFileObject::fgetcsv(): the $escape parameter must be provided, as its default value will change, either explicitly or via SplFileObject::setCsvControl() in %s on line %d
array(3) {
  [0]=>
  string(4) "aa\""
  [1]=>
  string(2) "bb"
  [2]=>
  string(3) "\"c"
}
