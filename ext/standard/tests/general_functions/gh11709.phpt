--TEST--
GH-11709 (Line breaks in quoted strings fail with INI_SCANNER_RAW)
--FILE--
<?php
$ini = <<<INI
a=42
b="line 1
line 2"
c="x
y" ; comment
d=1
INI;
var_dump(parse_ini_string($ini, false, INI_SCANNER_RAW));
var_dump(parse_ini_string($ini, false, INI_SCANNER_RAW) === parse_ini_string($ini, false, INI_SCANNER_NORMAL));

echo "CRLF:\n";
var_dump(parse_ini_string("c=\"x\r\ny\"\r\nd=1\r\n", false, INI_SCANNER_RAW) === ["c" => "x\r\ny", "d" => "1"]);

echo "Unclosed quote:\n";
var_dump(parse_ini_string("a=42\nb=\"open\nz=1\n", false, INI_SCANNER_RAW));

echo "Line numbers:\n";
var_dump(parse_ini_string("b=\"x\ny\"\nc=1\n[bad\n", false, INI_SCANNER_RAW));
?>
--EXPECTF--
array(4) {
  ["a"]=>
  string(2) "42"
  ["b"]=>
  string(13) "line 1
line 2"
  ["c"]=>
  string(3) "x
y"
  ["d"]=>
  string(1) "1"
}
bool(true)
CRLF:
bool(true)
Unclosed quote:
array(3) {
  ["a"]=>
  string(2) "42"
  ["b"]=>
  string(5) ""open"
  ["z"]=>
  string(1) "1"
}
Line numbers:

Warning: syntax error, unexpected end of file, expecting ']' in Unknown on line 4
 in %s on line %d
bool(false)
