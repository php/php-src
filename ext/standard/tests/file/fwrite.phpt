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
Warning: fwrite() expects at least 2 parameters, 1 given in %s on line %d
bool(false)

Warning: fwrite() expects parameter 2 to be string, array given in %s on line %d
bool(false)
int(0)
int(0)
int(4)
int(0)

Warning: fwrite() expects parameter 1 to be resource, array given in %s on line %d
bool(false)

Warning: fwrite() expects parameter 1 to be resource, array given in %s on line %d
bool(false)

Warning: fwrite() expects at least 2 parameters, 1 given in %s on line %d
bool(false)
string(4) "data"
Done
