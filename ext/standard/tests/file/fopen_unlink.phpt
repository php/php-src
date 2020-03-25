--TEST--
Test fopen() function : check fopen()ed descriptor is usable after the fs object is removed
--FILE--
<?php

var_dump(
    $p = __DIR__ . DIRECTORY_SEPARATOR . 'tututu',
    $f = fopen($p, 'w+'),
    unlink($p),
    file_exists($p),
    fwrite($f, 'hello'),
    fseek($f, 0),
    fread($f, 16),
    fwrite($f, 'world'),
    fseek($f, 0),
    fread($f, 16),
    fclose($f)
);

?>
--EXPECTF--
string(%d) "%stututu"
resource(%s) of type (Unknown)
bool(true)
bool(false)
int(5)
int(0)
string(5) "hello"
int(5)
int(0)
string(10) "helloworld"
bool(true)
