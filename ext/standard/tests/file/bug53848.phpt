--TEST--
Bug #53848 (fgetcsv removes leading spaces from fields)
--FILE--
<?php
$file = __DIR__ . "/bug53848.csv";
@unlink($file);
file_put_contents($file, "a,b\n  c,  d");
$fp = fopen($file, "r");
while ($l = fgetcsv($fp)) var_dump($l);
fclose($fp);
@unlink($file);
?>
--EXPECT--
array(2) {
  [0]=>
  string(1) "a"
  [1]=>
  string(1) "b"
}
array(2) {
  [0]=>
  string(3) "  c"
  [1]=>
  string(3) "  d"
}
