--TEST--
fputcsv(): Deprecation if using default escape arg
--FILE--
<?php
$line = "test1,test2";

$file = __DIR__ . '/fputcsv_default_escape_deprecation.csv';

$data = ["test1", "test2"];

$fp = fopen($file, 'w');

fputcsv($fp, $data);
unset($fp);

var_dump(file_get_contents($file));

?>
--CLEAN--
<?php
$file = __DIR__ . '/fputcsv_default_escape_deprecation.csv';
@unlink($file);
?>
--EXPECTF--
Deprecated: fputcsv(): the $escape parameter must be provided as its default value will change in %s on line %d
string(12) "test1,test2
"
