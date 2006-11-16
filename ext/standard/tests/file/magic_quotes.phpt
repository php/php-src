--TEST--
various magic quotes tests
--FILE--
<?php

$filename = dirname(__FILE__)."/magic_quotes.dat";

var_dump(get_magic_quotes_runtime());
var_dump(get_magic_quotes_runtime("blah"));

file_put_contents($filename, "some'content'here\"and}there");

var_dump(set_magic_quotes_runtime(true));
var_dump(get_magic_quotes_runtime());
var_dump(set_magic_quotes_runtime(-1));
var_dump(get_magic_quotes_runtime());

var_dump(file_get_contents($filename));

var_dump(set_magic_quotes_runtime(0));
var_dump(get_magic_quotes_runtime());

var_dump(file_get_contents($filename));

var_dump(set_magic_quotes_runtime(1));
var_dump(get_magic_quotes_runtime());

file_put_contents($filename, "some'content'here\"and}there");

var_dump(file_get_contents($filename));

var_dump(set_magic_quotes_runtime(0));
var_dump(set_magic_quotes_runtime());
var_dump(get_magic_quotes_runtime());

var_dump(file_get_contents($filename));


@unlink($filename);

echo "Done\n";
?>
--EXPECTF--	
int(0)
int(0)
bool(true)
int(1)
bool(true)
int(1)
string(30) "some\'content\'here\"and}there"
bool(true)
int(0)
string(27) "some'content'here"and}there"
bool(true)
int(1)
string(30) "some\'content\'here\"and}there"
bool(true)
bool(false)
int(0)
string(27) "some'content'here"and}there"
Done
