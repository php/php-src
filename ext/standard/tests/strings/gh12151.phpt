--TEST--
GH-12151 (str_getcsv ending with escape zero segfualt)
--FILE--
<?php
var_export(str_getcsv("y","","y","\000"));
var_export(str_getcsv("\0yy","y","y","\0"));
?>
--EXPECT--
array (
  0 => '' . "\0" . '',
)array (
  0 => '' . "\0" . '',
  1 => '' . "\0" . '',
)
