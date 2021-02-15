--TEST--
Bug #80724 (FOLLOW_SYMLINKS interfering with FilesystemIterator key flags)
--FILE--
<?php
$iterator = new FilesystemIterator(__DIR__, FilesystemIterator::KEY_AS_FILENAME);
foreach ($iterator as $key => $value) {
    echo var_dump(str_contains($key, __DIR__ . DIRECTORY_SEPARATOR));
    break;
}
$iterator->rewind();
echo var_dump(str_contains($iterator->key(), __DIR__ . DIRECTORY_SEPARATOR));

$iterator2 = new FilesystemIterator(__DIR__, FilesystemIterator::FOLLOW_SYMLINKS | FilesystemIterator::KEY_AS_FILENAME);
foreach ($iterator2 as $key => $value) {
    echo var_dump(str_contains($key, __DIR__ . DIRECTORY_SEPARATOR));
    break;
}
$iterator2->rewind();
echo var_dump(str_contains($iterator2->key(), __DIR__ . DIRECTORY_SEPARATOR));
?>
--EXPECT--
bool(false)
bool(false)
bool(false)
bool(false)