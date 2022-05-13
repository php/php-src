--TEST--
Phar: tar with hard link to nowhere
--EXTENSIONS--
phar
--FILE--
<?php
$fname = __DIR__ . '/' . basename(__FILE__, '.php') . '.tar';
$pname = 'phar://' . $fname;

include __DIR__ . '/files/corrupt_tarmaker.php.inc';
$a = new corrupt_tarmaker($fname, 'none');
$a->init();
$a->addFile('hardlink', 'internal/file.txt', array(
                    'mode' => 0xA000 + 0644,
                    'uid' => 0,
                    'gid' => 0,
                    'size' => 0,
                    'mtime' => time(),
                ));
$a->close();

try {
    $p = new PharData($fname);
} catch (Exception $e) {
    echo $e->getMessage() . "\n";
}
?>
--CLEAN--
<?php
unlink(__DIR__ . '/' . basename(__FILE__, '.clean.php') . '.tar');
?>
--EXPECTF--
phar error: "%slinks2.tar" is a corrupted tar file - hard link to non-existent file "internal/file.txt"
