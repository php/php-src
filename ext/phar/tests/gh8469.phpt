--TEST--
GH-8469 (Phar::decompressFiles() and Phar::compressFiles() keep the old compressed contents)
--EXTENSIONS--
phar
zlib
bz2
--INI--
phar.require_hash=0
phar.readonly=0
--FILE--
<?php
$contents = str_repeat('four score and seven years ago ', 10);

foreach (['phar', 'phar.zip'] as $ext) {
    $fname = __DIR__ . "/gh8469.$ext";
    $phar = new Phar($fname);
    $phar['a.txt'] = $contents;
    $phar['b.txt'] = 'our fathers';

    $phar->compressFiles(Phar::GZ);
    $phar->compressFiles(Phar::BZ2);
    var_dump($phar['a.txt']->isCompressed(Phar::BZ2));
    var_dump($phar['a.txt']->getContent() === $contents);

    $phar->decompressFiles();
    var_dump($phar['a.txt']->isCompressed());
    var_dump($phar['a.txt']->getContent() === $contents);
    var_dump($phar['b.txt']->getContent());
    unset($phar);

    $phar = new Phar($fname);
    var_dump($phar['a.txt']->getContent() === $contents);
    var_dump(file_get_contents("phar://$fname/b.txt"));
    unset($phar);
}
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/gh8469.phar');
@unlink(__DIR__ . '/gh8469.phar.zip');
?>
--EXPECT--
bool(true)
bool(true)
bool(false)
bool(true)
string(11) "our fathers"
bool(true)
string(11) "our fathers"
bool(true)
bool(true)
bool(false)
bool(true)
string(11) "our fathers"
bool(true)
string(11) "our fathers"
