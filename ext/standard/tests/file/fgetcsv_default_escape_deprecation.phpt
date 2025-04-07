--TEST--
fgetcsv(): Deprecation if using default escape arg
--FILE--
<?php
$line = "test1,test2";

$file = __DIR__ . '/fgetcsv_default_escape_deprecation.csv';

$fp = fopen($file, "w");
fwrite($fp, $line);
fclose($fp);

var_dump(fgetcsv(fopen($file, "r"), 1024));
?>
--CLEAN--
<?php
$file = __DIR__ . '/fgetcsv_default_escape_deprecation.csv';
@unlink($file);
?>
--EXPECTF--
Deprecated: fgetcsv(): the $escape parameter must be provided as its default value will change in %s on line %d
array(2) {
  [0]=>
  string(5) "test1"
  [1]=>
  string(5) "test2"
}
