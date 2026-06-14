--TEST--
GH-18267 finfo_file() assertion trigger on NULL stream context
--EXTENSIONS--
fileinfo
--FILE--
<?php
$cls = new finfo();
try {
    $cls->file("test",FILEINFO_NONE, STDERR);
} catch (\TypeError $e) {
    echo $e->getMessage(); 
}
--EXPECT--
finfo::file(): supplied resource is not a valid Stream-Context resource
