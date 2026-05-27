--TEST--
Bug GH-22156 SplFileObject::fgetcsv() with named escape parameter
--FILE--
<?php

$file = new SplFileObject('php://memory', 'rw+');

$file->fwrite("foo;bar;baz");
$file->seek(0);

$file->setCsvControl(';', "\n", "\\");

while (!$file->eof()) {
    $line = $file->fgetcsv(escape: '\\');
    var_dump($line);
}

?>
--EXPECT--
array(3) {
  [0]=>
  string(3) "foo"
  [1]=>
  string(3) "bar"
  [2]=>
  string(3) "baz"
}

