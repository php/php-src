--TEST--
Bug #81477 (LimitIterator + SplFileObject regression in 8.0.1)
--FILE--
<?php
$filename = __DIR__ . '/bug81477.csv';

$s = fopen($filename, 'w+');
fwrite($s, "foo,bar\nbaz,bat\nmore,data\n");
fclose($s);

$sfo = new SplFileObject($filename);
$sfo->setFlags(SplFileObject::READ_AHEAD);
$limitIter = new LimitIterator($sfo, 1, -1);

foreach($limitIter as $row) {
    var_dump($row);
}
?>
--EXPECT--
string(8) "baz,bat
"
string(10) "more,data
"
string(0) ""
--CLEAN--
<?php
@unlink(__DIR__ . '/bug81477.csv');
?>
