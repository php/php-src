--TEST--
SplFileObject::fgetcsv() enclosure error
--FILE--
<?php
$fp = fopen('SplFileObject__fgetcsv5.csv', 'w+');
fputcsv(
    $fp,
    [
        'field1',
        'field2',
        'field3',
        5,
    ],
    escape: '',
);
fclose($fp);

$fo = new SplFileObject('SplFileObject__fgetcsv5.csv');
$fo->setCsvControl(escape: '');
try {
    var_dump($fo->fgetcsv(enclosure: 'invalid'));
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}
?>
--CLEAN--
<?php
unlink('SplFileObject__fgetcsv5.csv');
?>
--EXPECT--
SplFileObject::fgetcsv(): Argument #2 ($enclosure) must be a single character
