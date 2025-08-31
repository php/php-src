--TEST--
Bug #80724 (FOLLOW_SYMLINKS interfering with FilesystemIterator key flags)
--FILE--
<?php
$iterator = new FilesystemIterator(__DIR__, FilesystemIterator::KEY_AS_FILENAME);
foreach ($iterator as $key => $value) {
    echo var_dump(hasSeparator($key));
    break;
}
$iterator->rewind();
echo var_dump(hasSeparator($iterator->key()));

$iterator->setFlags(0);
echo var_dump(hasSeparator($iterator->key()));

$iterator->setFlags(FilesystemIterator::KEY_AS_FILENAME);
echo var_dump(hasSeparator($iterator->key()));

$iterator2 = new FilesystemIterator(__DIR__, FilesystemIterator::FOLLOW_SYMLINKS | FilesystemIterator::KEY_AS_FILENAME);
foreach ($iterator2 as $key => $value) {
    echo var_dump(hasSeparator($key));
    break;
}
$iterator2->rewind();
echo var_dump(hasSeparator($iterator2->key()));

$iterator2->setFlags(0);
echo var_dump(hasSeparator($iterator2->key()));

$iterator2->setFlags(FilesystemIterator::KEY_AS_FILENAME);
echo var_dump(hasSeparator($iterator2->key()));

function hasSeparator($key) {
    return str_contains($key, __DIR__ . DIRECTORY_SEPARATOR);
}

?>
--EXPECT--
bool(false)
bool(false)
bool(true)
bool(false)
bool(false)
bool(false)
bool(true)
bool(false)
