--TEST--
Phar: tar-based phar, third-party tar with no stub, Phar->getStub()
--EXTENSIONS--
phar
--INI--
phar.readonly=1
phar.require_hash=0
--FILE--
<?php
include __DIR__ . '/files/tarmaker.php.inc';
$fname = __DIR__ . '/tar_nostub.phar.tar';
$alias = 'phar://' . $fname;
$fname2 = __DIR__ . '/tar_nostub.tar';

$tar = new tarmaker($fname, 'none');
$tar->init();
$tar->addFile('tar_004.php', '<?php var_dump(__FILE__);');
$tar->addFile('internal/file/here', "hi there!\n");
$tar->close();

try {
    $phar = new Phar($fname);
    var_dump($phar->getStub());
} catch (Exception $e) {
    echo $e->getMessage()."\n";
}

copy($fname, $fname2);

try {
    $phar = new PharData($fname2);
    var_dump($phar->getStub());
} catch (Exception $e) {
    echo $e->getMessage()."\n";
}

?>
--CLEAN--
<?php
@unlink(__DIR__ . '/tar_nostub.phar.tar');
@unlink(__DIR__ . '/tar_nostub.tar');
?>
--EXPECTF--
RecursiveDirectoryIterator::__construct(phar://%star_nostub.phar.tar/): Failed to open directory: '%star_nostub.phar.tar' is not a phar archive. Use PharData::__construct() for a standard zip or tar archive
phar url "phar://%star_nostub.phar.tar/" is unknown
string(0) ""
