--TEST--
Path edge case
--EXTENSIONS--
phar
--FILE--
<?php

$tar = new PharData(__DIR__ . '/../files/gh19311/path_edge_case.tar');
foreach (new RecursiveIteratorIterator($tar) as $file) {
    echo $file->getPathname(), "\n";
}

?>
--EXPECTF--
Notice: PharData::__construct(): File PAX header component not understood: invalid path length in %s on line %d
phar://%s/path_edge_case.tar/example.txt
