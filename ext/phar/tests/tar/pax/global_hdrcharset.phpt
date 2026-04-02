--TEST--
Global hdrcharset
--EXTENSIONS--
phar
--FILE--
<?php

$tar = new PharData(__DIR__ . '/../files/gh19311/global_hdrcharset.tar');
foreach (new RecursiveIteratorIterator($tar) as $file) {
    echo $file->getPathname(), "\n";
}

?>
--EXPECTF--
Notice: PharData::__construct(): Global PAX header component not understood: invalid header character set in %s on line %d
phar://%s/global_hdrcharset.tar/example.txt
