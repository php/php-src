--TEST--
Phar: tar-based phar corrupted
--EXTENSIONS--
phar
--FILE--
<?php
include __DIR__ . '/files/make_invalid_tar.php.inc';

$tar = new corrupter(__DIR__ . '/tar_001.phar.tar', 'none');
$tar->init();
$tar->addFile('tar_001.phpt', __FILE__);
$tar->close();

$tar = fopen('phar://' . __DIR__ . '/tar_001.phar.tar/tar_001.phpt', 'rb');
try {
    $phar = new Phar(__DIR__ . '/tar_001.phar.tar');
    echo "should not execute\n";
} catch (Exception $e) {
    echo $e->getMessage() . "\n";
}
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/tar_001.phar.tar');
?>
--EXPECTF--
Warning: fopen(phar://%star_001.phar.tar/tar_001.phpt): Failed to open stream: phar error: "%star_001.phar.tar" is a corrupted tar file (truncated) in %star_001.php on line 9
phar error: "%star_001.phar.tar" is a corrupted tar file (truncated)
