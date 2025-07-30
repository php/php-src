--TEST--
Local hdrcharset
--EXTENSIONS--
phar
--FILE--
<?php

$tar = new PharData(__DIR__ . '/../files/gh19311/local_hdrcharset.tar');
foreach (new RecursiveIteratorIterator($tar) as $file) {
    echo $file->getPathname(), "\n";
}

?>
--EXPECTF--
Notice: PharData::__construct(): File PAX header component not understood: invalid header character set in %s on line %d
phar://%s/local_hdrcharset.tar/example.txt
