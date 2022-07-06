--TEST--
Bug #41964 (strtotime returns a timestamp for non-time string of pattern '(A|a) .+')
--FILE--
<?php
date_default_timezone_set("UTC");

error_reporting(0);

$res = date_parse('Ask the Experts');
var_dump($res['zone'], $res['tz_abbr']);
echo "\n";

$res = date_parse('A ');
var_dump($res['zone'], $res['tz_abbr']);
echo "\n";

$res = date_parse('A');
var_dump($res['zone'], $res['tz_abbr']);
echo "\n";

$res = date_parse('a ');
var_dump($res['zone'], $res['tz_abbr']);
echo "\n";

$res = date_parse('a');
var_dump($res['zone'], $res['tz_abbr']);
echo "\n";

$res = date_parse('A Revolution in Development');
var_dump($res['zone'], $res['tz_abbr']);
echo "\n";

$res = date_parse('a nothing');
var_dump($res['zone'], $res['tz_abbr']);
echo "\n";


?>
--EXPECT--
NULL
NULL

int(3600)
string(1) "A"

int(3600)
string(1) "A"

int(3600)
string(1) "A"

int(3600)
string(1) "A"

int(3600)
string(1) "A"

int(3600)
string(1) "A"
