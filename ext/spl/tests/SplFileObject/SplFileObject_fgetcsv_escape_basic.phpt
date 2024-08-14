--TEST--
SplFileObject::fgetcsv with alternative delimiter
--FILE--
<?php
$fp = fopen('SplFileObject__fgetcsv6.csv', 'w+');
fwrite($fp, '"aaa","b""bb","ccc"');
fclose($fp);

$fo = new SplFileObject('SplFileObject__fgetcsv6.csv');
var_dump($fo->fgetcsv(',', '"', '"'));
?>
--CLEAN--
<?php
unlink('SplFileObject__fgetcsv6.csv');
?>
--EXPECTF--
Deprecated: SplFileObject::fgetcsv(): Passing a non-empty string to the $escape parameter is deprecated since 8.4 in %s on line %d
array(3) {
  [0]=>
  string(3) "aaa"
  [1]=>
  string(4) "b"bb"
  [2]=>
  string(3) "ccc"
}
