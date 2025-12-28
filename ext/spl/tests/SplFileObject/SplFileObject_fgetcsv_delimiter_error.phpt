--TEST--
SplFileObject::fgetcsv() delimiter error
--FILE--
<?php
$fp = fopen('SplFileObject__fgetcsv3.csv', 'w+');
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

$fo = new SplFileObject('SplFileObject__fgetcsv3.csv');
$fo->setCsvControl(escape: '');
try {
    var_dump($fo->fgetcsv('invalid'));
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}
?>
--CLEAN--
<?php
unlink('SplFileObject__fgetcsv3.csv');
?>
--EXPECT--
SplFileObject::fgetcsv(): Argument #1 ($separator) must be a single character
