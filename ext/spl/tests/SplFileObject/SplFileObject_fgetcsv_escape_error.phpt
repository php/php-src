--TEST--
SplFileObject::fgetcsv with alternative delimiter
--FILE--
<?php
$fp = fopen('SplFileObject__fgetcsv8.csv', 'w+');
fwrite($fp, '"aaa","b""bb","ccc"');
fclose($fp);

$fo = new SplFileObject('SplFileObject__fgetcsv8.csv');
try {
    var_dump($fo->fgetcsv(',', '"', 'invalid'));
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}
?>
--CLEAN--
<?php
unlink('SplFileObject__fgetcsv8.csv');
?>
--EXPECT--
SplFileObject::fgetcsv(): Argument #3 ($escape) must be empty or a single character
