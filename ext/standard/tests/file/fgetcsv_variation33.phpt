--TEST--
fgetcsv() with unterminated enclosure at the end of file
--FILE--
<?php
$contents = <<<EOS
"cell1","cell2"
"cell1","
EOS;
$stream = fopen('php://memory', 'w+');
fwrite($stream, $contents);
rewind($stream);
while (($data = fgetcsv($stream)) !== false) {
    var_dump($data);
}
fclose($stream);
?>
--EXPECT--
array(2) {
  [0]=>
  string(5) "cell1"
  [1]=>
  string(5) "cell2"
}
array(2) {
  [0]=>
  string(5) "cell1"
  [1]=>
  string(0) ""
}
