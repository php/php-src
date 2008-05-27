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
var_dump(fwrite($fp, b"data"));

$fp = fopen($filename, "w");
var_dump(fwrite($fp, b"data", -1));
var_dump(fwrite($fp, b"data", 100000));
fclose($fp);

var_dump(fwrite($fp, b"data", -1));

var_dump(fwrite(array(), b"data", -1));
var_dump(fwrite(array(), b"data"));
var_dump(fwrite(array()));

var_dump(file_get_contents($filename));

@unlink($filename);
echo "Done\n";
?>
--EXPECTF--
Warning: fwrite() expects at least 2 parameters, 1 given in %s on line %d
NULL

Notice: Array to string conversion in %s on line %d
int(5)
int(0)
int(0)
int(4)

Warning: fwrite(): 7 is not a valid stream resource in %s on line %d
bool(false)

Warning: fwrite() expects parameter 1 to be resource, array given in %s on line %d
NULL

Warning: fwrite() expects parameter 1 to be resource, array given in %s on line %d
NULL

Warning: fwrite() expects at least 2 parameters, 1 given in %s on line %d
NULL
string(4) "data"
Done
