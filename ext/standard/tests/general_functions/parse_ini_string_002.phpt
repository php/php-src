--TEST--
parse_ini_string() multiple calls
--FILE--
<?php

$ini = "
test =
";
var_dump(parse_ini_string($ini));
$ini = "
test==
";
var_dump(parse_ini_string($ini));

$ini = "
test=test=
";
var_dump(parse_ini_string($ini));

$ini = "
test= \"new
line\"
";
var_dump(parse_ini_string($ini));

define("TEST_CONST", "test const value");
$ini = "
test=TEST_CONST
";
var_dump(parse_ini_string($ini));

$ini = "
[section]
test=hello
";
var_dump(parse_ini_string($ini, true));

$ini = "
[section]
test=hello
";
var_dump(parse_ini_string($ini, false));

$ini = "
section.test=hello
";
var_dump(parse_ini_string($ini, true));

$ini = "
[section]
section.test=hello
";
var_dump(parse_ini_string($ini, true));

$ini = "
[section]
1=2
";
var_dump(parse_ini_string($ini, true));

$ini = "
1=2
";
var_dump(parse_ini_string($ini, true));
$ini = "
test=test2
test=test3
test=test4
";
var_dump(parse_ini_string($ini, true));

/* From bug #44574 */
$ini = "[section1]\nname = value";
var_dump(parse_ini_string($ini, true));

/* #44842, labels starting with underscore */
$ini = <<<'INI'
foo=bar1
_foo=bar2
foo_=bar3
INI;
var_dump(parse_ini_string($ini, true));

echo "Done\n";
?>
--EXPECTF--
array(1) {
  ["test"]=>
  string(0) ""
}

Warning: syntax error, unexpected '='%sin Unknown on line 2
 in %s
bool(false)

Warning: syntax error, unexpected '='%sin Unknown on line 2
 in %s
bool(false)
array(1) {
  ["test"]=>
  string(8) "new
line"
}
array(1) {
  ["test"]=>
  string(16) "test const value"
}
array(1) {
  ["section"]=>
  array(1) {
    ["test"]=>
    string(5) "hello"
  }
}
array(1) {
  ["test"]=>
  string(5) "hello"
}
array(1) {
  ["section.test"]=>
  string(5) "hello"
}
array(1) {
  ["section"]=>
  array(1) {
    ["section.test"]=>
    string(5) "hello"
  }
}
array(1) {
  ["section"]=>
  array(1) {
    [1]=>
    string(1) "2"
  }
}
array(1) {
  [1]=>
  string(1) "2"
}
array(1) {
  ["test"]=>
  string(5) "test4"
}
array(1) {
  ["section1"]=>
  array(1) {
    ["name"]=>
    string(5) "value"
  }
}
array(3) {
  ["foo"]=>
  string(4) "bar1"
  ["_foo"]=>
  string(4) "bar2"
  ["foo_"]=>
  string(4) "bar3"
}
Done
