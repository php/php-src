--TEST--
Bug #74719 Allow NULL as context, testing fopen, rename, unlink, mkdir and rmdir
--CREDITS--
Alexander Holman <alexander@holman.org.uk>
--FILE--
<?php

// fopen
$tmpFile = __DIR__ . "/bug74719.tmp";
$h = fopen($tmpFile, "w", false, NULL);
if ($h !== false) {
    echo "ok\n";
    fclose($h);
}

$newTmpFile = __DIR__ . "/bug74719_renamed.tmp";
if (rename($tmpFile, $newTmpFile, NULL)) {
    echo "ok\n";
}

if (unlink($newTmpFile, NULL)) {
    echo "ok\n";
}

$tmpDir = __DIR__ . "/bug74719_dir";

if (mkdir($tmpDir, 0777, false, NULL)) {
    echo "ok\n";
}

if (rmdir($tmpDir, NULL)) {
    echo "ok\n";
}

?>
--EXPECT--
ok
ok
ok
ok
ok
