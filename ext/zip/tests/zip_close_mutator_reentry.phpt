--TEST--
ZipArchive mutators throw when called from a progress callback during close()
--EXTENSIONS--
zip
--SKIPIF--
<?php
if (!method_exists(ZipArchive::class, 'registerProgressCallback')) {
    die('skip progress callbacks are not supported');
}
if (!method_exists(ZipArchive::class, 'registerCancelCallback')) {
    die('skip cancel callbacks are not supported');
}
if (!method_exists(ZipArchive::class, 'setEncryptionName')) {
    die('skip encryption not supported');
}
if (!method_exists(ZipArchive::class, 'setMtimeName')) {
    die('skip libzip too old');
}
?>
--FILE--
<?php
$filename = __DIR__ . '/zip_close_mutator_reentry.zip';

function populate(ZipArchive $zip, string $filename): void {
    $zip->open($filename, ZipArchive::CREATE | ZipArchive::OVERWRITE);
    $zip->addFromString('a.txt', str_repeat('a', 100000));
    $zip->addFromString('b.txt', str_repeat('b', 100000));
}

$zip = new ZipArchive();
populate($zip, $filename);
$zip->registerProgressCallback(0.0, function ($rate) use ($zip) {
    static $done = false;
    if ($done || $rate <= 0) {
        return;
    }
    $done = true;
    try {
        var_dump($zip->deleteIndex(0));
    } catch (Error $e) {
        echo $e::class, ': ', $e->getMessage(), PHP_EOL;
    }
});
var_dump($zip->close());

$zip = new ZipArchive();
populate($zip, $filename);
$mutators = [
    'deleteIndex' => fn() => $zip->deleteIndex(0),
    'deleteName' => fn() => $zip->deleteName('a.txt'),
    'unchangeIndex' => fn() => $zip->unchangeIndex(0),
    'unchangeName' => fn() => $zip->unchangeName('a.txt'),
    'unchangeAll' => fn() => $zip->unchangeAll(),
    'unchangeArchive' => fn() => $zip->unchangeArchive(),
    'addEmptyDir' => fn() => $zip->addEmptyDir('dir'),
    'addFile' => fn() => $zip->addFile(__FILE__, 'file.phpt'),
    'addFromString' => fn() => $zip->addFromString('c.txt', 'c'),
    'addGlob' => fn() => $zip->addGlob(__FILE__),
    'addPattern' => fn() => $zip->addPattern('/\.phpt$/', __DIR__),
    'replaceFile' => fn() => $zip->replaceFile(__FILE__, 0),
    'renameIndex' => fn() => $zip->renameIndex(0, 'x.txt'),
    'renameName' => fn() => $zip->renameName('a.txt', 'x.txt'),
    'setArchiveComment' => fn() => $zip->setArchiveComment('comment'),
    'setArchiveFlag' => fn() => $zip->setArchiveFlag(ZipArchive::AFL_RDONLY, 1),
    'setCommentIndex' => fn() => $zip->setCommentIndex(0, 'comment'),
    'setCommentName' => fn() => $zip->setCommentName('a.txt', 'comment'),
    'setCompressionIndex' => fn() => $zip->setCompressionIndex(0, ZipArchive::CM_STORE),
    'setCompressionName' => fn() => $zip->setCompressionName('a.txt', ZipArchive::CM_STORE),
    'setEncryptionIndex' => fn() => $zip->setEncryptionIndex(0, ZipArchive::EM_AES_256, 'secret'),
    'setEncryptionName' => fn() => $zip->setEncryptionName('a.txt', ZipArchive::EM_AES_256, 'secret'),
    'setExternalAttributesIndex' => fn() => $zip->setExternalAttributesIndex(0, ZipArchive::OPSYS_UNIX, 0),
    'setExternalAttributesName' => fn() => $zip->setExternalAttributesName('a.txt', ZipArchive::OPSYS_UNIX, 0),
    'setMtimeIndex' => fn() => $zip->setMtimeIndex(0, 0),
    'setMtimeName' => fn() => $zip->setMtimeName('a.txt', 0),
    'setPassword' => fn() => $zip->setPassword('secret'),
    'registerCancelCallback' => fn() => $zip->registerCancelCallback(fn() => 0),
    'registerProgressCallback' => fn() => $zip->registerProgressCallback(0.5, function () {}),
];

$zip->registerProgressCallback(0.0, function ($rate) use ($mutators) {
    static $done = false;
    if ($done || $rate <= 0) {
        return;
    }
    $done = true;
    foreach ($mutators as $name => $mutator) {
        try {
            var_dump($mutator());
        } catch (Error $e) {
            echo $name, ': ', $e::class, ': ', $e->getMessage(), PHP_EOL;
        }
    }
});
var_dump($zip->close());

$zip = new ZipArchive();
var_dump($zip->open($filename, ZipArchive::CHECKCONS));
var_dump($zip->numFiles);
var_dump($zip->getFromName('a.txt') === str_repeat('a', 100000));
var_dump($zip->getFromName('b.txt') === str_repeat('b', 100000));
var_dump($zip->getArchiveComment());
$zip->close();
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/zip_close_mutator_reentry.zip');
?>
--EXPECT--
Error: Already being closed
bool(true)
deleteIndex: Error: Already being closed
deleteName: Error: Already being closed
unchangeIndex: Error: Already being closed
unchangeName: Error: Already being closed
unchangeAll: Error: Already being closed
unchangeArchive: Error: Already being closed
addEmptyDir: Error: Already being closed
addFile: Error: Already being closed
addFromString: Error: Already being closed
addGlob: Error: Already being closed
addPattern: Error: Already being closed
replaceFile: Error: Already being closed
renameIndex: Error: Already being closed
renameName: Error: Already being closed
setArchiveComment: Error: Already being closed
setArchiveFlag: Error: Already being closed
setCommentIndex: Error: Already being closed
setCommentName: Error: Already being closed
setCompressionIndex: Error: Already being closed
setCompressionName: Error: Already being closed
setEncryptionIndex: Error: Already being closed
setEncryptionName: Error: Already being closed
setExternalAttributesIndex: Error: Already being closed
setExternalAttributesName: Error: Already being closed
setMtimeIndex: Error: Already being closed
setMtimeName: Error: Already being closed
setPassword: Error: Already being closed
registerCancelCallback: Error: Already being closed
registerProgressCallback: Error: Already being closed
bool(true)
bool(true)
int(2)
bool(true)
bool(true)
string(0) ""
