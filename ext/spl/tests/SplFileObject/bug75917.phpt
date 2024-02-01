--TEST--
Bug #75917 (SplFileObject::seek broken with CSV flags)
--FILE--
<?php
$expected = [
    ['john', 'doe', 'john.doe@example.com', '0123456789'],
    ['jane', 'doe', 'jane.doe@example.com'],
];

$tmp = new SplTempFileObject();
foreach ($expected as $row) {
    $tmp->fputcsv($row);
}
$tmp->setFlags(0);
$tmp->seek(23);
var_dump($tmp->current());

$tmp->setFlags(SplFileObject::READ_CSV | SplFileObject::SKIP_EMPTY);
$tmp->seek(23);
var_dump($tmp->current());
?>
--EXPECT--
bool(false)
bool(false)
