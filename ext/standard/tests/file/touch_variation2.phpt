--TEST--
touch() - ensure touch does not delete existing file.
--CREDITS--
Dave Kelsey <d_kelsey@uk.ibm.com>
--FILE--
<?php


$filename = __DIR__."/touch_variation2.dat";
$fp=fopen($filename,"w");
fwrite ($fp,"mydata");
fclose($fp);

var_dump(touch($filename, 101));
var_dump(file_get_contents($filename));

@unlink($filename);
echo "Done\n";

?>
--EXPECT--
bool(true)
string(6) "mydata"
Done
