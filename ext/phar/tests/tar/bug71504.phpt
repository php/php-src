--TEST--
Bug #71504: Parsing of tar file with duplicate filenames causes memory leak
--EXTENSIONS--
phar
zlib
--FILE--
<?php
$fname = str_replace('\\', '/', __DIR__ . '/files/HTML_CSS-1.5.4.tgz');
try {
    $tar = new PharData($fname);
} catch(Exception $e) {
    echo $e->getMessage() . "\n";
}
?>
===DONE===
--EXPECT--
===DONE===
