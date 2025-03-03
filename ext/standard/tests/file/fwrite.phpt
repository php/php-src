--TEST--
fwrite() tests
--FILE--
<?php

$filename = __DIR__."/fwrite.dat";

$fp = fopen($filename, "w");
var_dump(fwrite($fp, ""));
fclose($fp);

$fp = fopen($filename, "r");
var_dump(fwrite($fp, "data"));

$fp = fopen($filename, "w");
var_dump(fwrite($fp, "data", -1));
var_dump(fwrite($fp, "data", 100000));
fclose($fp);

try {
    var_dump(fwrite($fp, "data", -1));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

var_dump(file_get_contents($filename));

echo "Done\n";
?>
--CLEAN--
<?php
$filename = __DIR__."/fwrite.dat";
@unlink($filename);
?>
--EXPECTF--
int(0)

Notice: fwrite(): Write of 4 bytes failed with errno=9 Bad file descriptor in %s on line %d
bool(false)
int(0)
int(4)
TypeError: fwrite(): supplied resource is not a valid stream resource
string(4) "data"
Done
