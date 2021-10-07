--TEST--
Bug #66588 SplFileObject::fgetcsv incorrectly returns a row on premature EOF
--DESCRIPTION--
This bug is about the different behavior with and without trailing newline both should behave the same,
and neither should return FALSE.
--FILE--
<?php

$s = fopen("php://memory", "w+");
fwrite($s, "\",bar");
rewind($s);
var_dump(fgetcsv($s));
fclose($s);

$s = fopen("php://memory", "w+");
fwrite($s, "\",bar\n");
rewind($s);
var_dump(fgetcsv($s));
fclose($s);
?>
--EXPECT--
array(1) {
  [0]=>
  string(4) ",bar"
}
array(1) {
  [0]=>
  string(5) ",bar
"
}
