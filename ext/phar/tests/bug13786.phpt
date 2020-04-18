--TEST--
Phar: bug #13786: "PHP crashes on phar recreate after unlink"
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--INI--
phar.require_hash=0
phar.readonly=0
--FILE--
<?php

try {
for ($i = 0; $i < 2; $i++) {
    $fname = "DataArchive.phar";
    $path = __DIR__ . DIRECTORY_SEPARATOR . $fname;
    $phar = new Phar($path);
    $phar->addFromString($i, "file $i in $fname");
    var_dump(file_get_contents($phar[$i]));
    unset($phar);
    unlink($path);
}

echo("\nWritten files: $i\n");
} catch (Exception $e) {
echo $e->getMessage() . "\n";
}

?>
--EXPECTF--
string(26) "file 0 in DataArchive.phar"
unable to seek to start of file "0" while creating new phar "%sDataArchive.phar"
