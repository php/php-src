--TEST--
SplFileObject::fgetcsv() with empty $escape
--FILE--
<?php
$contents = <<<EOS
"cell1","cell2\\","cell3","cell4"
"\\\\\\line1
line2\\\\\\"
EOS;
$file = new SplTempFileObject;
$file->fwrite($contents);
$file->rewind();
while (($data = $file->fgetcsv(',', '"', ''))) {
    var_dump($data);
}
?>
--EXPECTF--
Deprecated: Method SplFileObject::fgetcsv() is deprecated since 8.6 in %s on line %d
array(4) {
  [0]=>
  string(5) "cell1"
  [1]=>
  string(6) "cell2\"
  [2]=>
  string(5) "cell3"
  [3]=>
  string(5) "cell4"
}

Deprecated: Method SplFileObject::fgetcsv() is deprecated since 8.6 in %s on line %d
array(1) {
  [0]=>
  string(17) "\\\line1
line2\\\"
}

Deprecated: Method SplFileObject::fgetcsv() is deprecated since 8.6 in %s on line %d
