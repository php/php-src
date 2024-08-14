--TEST--
GH-12151 (str_getcsv ending with escape zero segfualt)
--FILE--
<?php
var_export(str_getcsv("y",",","y","\000"));
var_export(str_getcsv("\0yy","y","y","\0"));
?>
--EXPECTF--
Deprecated: str_getcsv(): Passing a non-empty string to the $escape parameter is deprecated since 8.4 in %s on line %d
array (
  0 => '' . "\0" . '',
)
Deprecated: str_getcsv(): Passing a non-empty string to the $escape parameter is deprecated since 8.4 in %s on line %d
array (
  0 => '' . "\0" . '',
  1 => '' . "\0" . '',
)
