--TEST--
Bug #69181 (READ_CSV|DROP_NEW_LINE drops newlines within fields)
--FILE--
<?php
$filename = __DIR__ . "/bug69181.csv";
$csv = <<<CSV
"foo\n\nbar\nbaz",qux

"foo\nbar\nbaz",qux
CSV;

file_put_contents($filename, $csv);

$file = new SplFileObject($filename);
$file->setFlags(SplFileObject::SKIP_EMPTY | SplFileObject::DROP_NEW_LINE | SplFileObject::READ_CSV);
var_dump(iterator_to_array($file));

echo "\n====\n\n";

$file->rewind();
while (($record = $file->fgetcsv())) {
  var_dump($record);
}
?>
--EXPECT--
array(2) {
  [0]=>
  array(2) {
    [0]=>
    string(12) "foo

bar
baz"
    [1]=>
    string(3) "qux"
  }
  [2]=>
  array(2) {
    [0]=>
    string(11) "foo
bar
baz"
    [1]=>
    string(3) "qux"
  }
}

====

array(2) {
  [0]=>
  string(12) "foo

bar
baz"
  [1]=>
  string(3) "qux"
}
array(2) {
  [0]=>
  string(11) "foo
bar
baz"
  [1]=>
  string(3) "qux"
}
--CLEAN--
<?php
@unlink(__DIR__ . "/bug69181.csv");
?>
