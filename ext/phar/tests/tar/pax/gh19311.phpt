--TEST--
GH-19311 (Unexpected path truncation of files contained in a tar file by PharData::extractTo())
--EXTENSIONS--
phar
--FILE--
<?php

$tar = new PharData(__DIR__ . '/../files/gh19311/pax_local_path_override.tar');
foreach (new RecursiveIteratorIterator($tar) as $file) {
    echo $file->getPathname(), "\n";
}

?>
--EXPECTF--
phar://%s/pax_local_path_override.tar/a-very-long-path/to-a-file/with-a-very-long-name/in-a-deep-directory-structure/a-php-file-with-a-very-long-name.php
