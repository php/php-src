--TEST--
parse_ini_string() multiple calls
--FILE--
<?php

var_dump(parse_ini_string());
var_dump(parse_ini_string(1,1,1,1));

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
Warning: parse_ini_string() expects at least 1 parameter, 0 given in %s
bool(false)

Warning: parse_ini_string() expects at most 3 parameters, 4 given in %s
bool(false)
array(1) {
  [%u|b%"test"]=>
  %unicode|string%(0) ""
}

Warning: syntax error, unexpected '='%sin Unknown on line 2
 in %s
bool(false)

Warning: syntax error, unexpected '='%sin Unknown on line 2
 in %s
bool(false)
array(1) {
  [%u|b%"test"]=>
  %unicode|string%(8) "new
line"
}
array(1) {
  [%u|b%"test"]=>
  %unicode|string%(16) "test const value"
}
array(1) {
  [%u|b%"section"]=>
  array(1) {
    [%u|b%"test"]=>
    %unicode|string%(5) "hello"
  }
}
array(1) {
  [%u|b%"test"]=>
  %unicode|string%(5) "hello"
}
array(1) {
  [%u|b%"section.test"]=>
  %unicode|string%(5) "hello"
}
array(1) {
  [%u|b%"section"]=>
  array(1) {
    [%u|b%"section.test"]=>
    %unicode|string%(5) "hello"
  }
}
array(1) {
  [%u|b%"section"]=>
  array(1) {
    [1]=>
    %unicode|string%(1) "2"
  }
}
array(1) {
  [1]=>
  %unicode|string%(1) "2"
}
array(1) {
  [%u|b%"test"]=>
  %unicode|string%(5) "test4"
}
array(1) {
  [%u|b%"section1"]=>
  array(1) {
    [%u|b%"name"]=>
    %unicode|string%(5) "value"
  }
}
array(3) {
  [%u|b%"foo"]=>
  %unicode|string%(4) "bar1"
  [%u|b%"_foo"]=>
  %unicode|string%(4) "bar2"
  [%u|b%"foo_"]=>
  %unicode|string%(4) "bar3"
}
Done
