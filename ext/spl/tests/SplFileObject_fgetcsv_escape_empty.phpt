--TEST--
SplFileObject::fgetcsv() with empty $escape
--FILE--
<?php
$contents = <<<EOS
"cell1","cell2\\","cell3","cell4"
"\\\\\\line1
line2\\\\\\"
EOS;
$file = new SplTempFileObject;
$file->fwrite($contents);
$file->rewind();
while (($data = $file->fgetcsv(',', '"', ''))) {
    print_r($data);
}
?>
===DONE===
--EXPECT--
Array
(
    [0] => cell1
    [1] => cell2\
    [2] => cell3
    [3] => cell4
)
Array
(
    [0] => \\\line1
line2\\\
)
===DONE===
