--TEST--
Bug #42229 (fgetcsv() behaves differently for a file containing '\n' with php5 and php6)
--FILE--
<?php

$file = dirname(__FILE__) .'/filetest.tmp';
$csv_string = '\n';

$fp = fopen($file, "w+t");
fwrite($fp, $csv_string);
rewind($fp);
var_dump(fgetcsv($fp));
fclose($fp);
unlink($file);

?>
--EXPECT--
array(1) {
  [0]=>
  unicode(2) "\n"
}
