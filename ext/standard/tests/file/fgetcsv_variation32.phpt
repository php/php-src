--TEST--
fgetcsv() with empty $escape
--FILE--
<?php
$contents = <<<EOS
"cell1","cell2\\","cell3","cell4"
"\\\\\\line1
line2\\\\\\"
EOS;
$stream = fopen('php://memory', 'w+');
fwrite($stream, $contents);
rewind($stream);
while (($data = fgetcsv($stream, 0, ',', '"', '')) !== false) {
    print_r($data);
}
fclose($stream);
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
