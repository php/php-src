--TEST--
fwrite() tests
--FILE--
<?php

$filename = dirname(__FILE__)."/fwrite.dat";

$fp = fopen($filename, "w");
var_dump(fwrite($fp));
var_dump(fwrite($fp, array()));
fclose($fp);

$fp = fopen($filename, "r");
var_dump(fwrite($fp, "data"));

$fp = fopen($filename, "w");
var_dump(fwrite($fp, "data", -1));
var_dump(fwrite($fp, "data", 100000));
fclose($fp);

var_dump(fwrite($fp, "data", -1));

var_dump(fwrite(array(), "data", -1));
var_dump(fwrite(array(), "data"));
var_dump(fwrite(array()));

var_dump(file_get_contents($filename));

@unlink($filename);
echo "Done\n";
?>
--EXPECTF--	
Warning: Wrong parameter count for fwrite() in %s on line %d
NULL

Notice: Array to string conversion in %s on line %d
int(5)
int(0)
int(0)
int(4)
int(0)
int(0)

Warning: fwrite(): supplied argument is not a valid stream resource in %s on line %d
bool(false)

Warning: Wrong parameter count for fwrite() in %s on line %d
NULL
string(4) "data"
Done
