--TEST--
Test function gzclose() by calling it with its expected arguments
--EXTENSIONS--
zlib
--FILE--
<?php
// note that gzclose is an alias to fclose. parameter checking tests will be
// the same as fclose

$f = __DIR__."/004.txt.gz";
$h = gzopen($f, 'r');
gzread($h, 20);
var_dump(gzclose($h));

//should fail.
try {
    gzread($h, 20);
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

$h = gzopen($f, 'r');
gzread($h, 20);
var_dump(fclose($h));

//should fail.
try {
    gzread($h, 20);
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}


?>
--EXPECT--
bool(true)
gzread(): supplied resource is not a valid stream resource
bool(true)
gzread(): supplied resource is not a valid stream resource
