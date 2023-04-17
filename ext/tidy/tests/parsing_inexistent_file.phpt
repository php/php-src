--TEST--
Trying to parse a non existent file
--EXTENSIONS--
tidy
--FILE--
<?php

$tidy = new tidy;
var_dump($tidy->parseFile("does_not_exist.html"));

var_dump(tidy_parse_file("does_not_exist.html"));

$tidy = new tidy("does_not_exist.html");
?>
--EXPECTF--
Warning: tidy::parseFile(): Cannot load "does_not_exist.html" into memory in %s on line %d
bool(false)

Warning: tidy_parse_file(): Cannot load "does_not_exist.html" into memory in %s on line %d
bool(false)

Warning: tidy::__construct(): Cannot load "does_not_exist.html" into memory in %s on line %d
