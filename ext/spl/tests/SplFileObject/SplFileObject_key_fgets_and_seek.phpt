--TEST--
SplFileObject verify interactions between seeking, getting the key and fgets
--FILE--
<?php

$file = new SplTempFileObject();

for ($i = 0; $i < 100; $i++) {
    $file->fwrite("Foo $i\n");
}

$file->seek(50);

var_dump(
    ['line' => $file->key(), 'contents' => trim($file->fgets())],
    ['line' => $file->key(), 'contents' => trim($file->fgets())],
    ['line' => $file->key(), 'contents' => trim($file->fgets())],
);

?>
--EXPECT--
array(2) {
  ["line"]=>
  int(50)
  ["contents"]=>
  string(6) "Foo 50"
}
array(2) {
  ["line"]=>
  int(51)
  ["contents"]=>
  string(6) "Foo 51"
}
array(2) {
  ["line"]=>
  int(52)
  ["contents"]=>
  string(6) "Foo 52"
}
