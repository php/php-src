--TEST--
parse_ini_file() tests
--FILE--
<?php

$filename = dirname(__FILE__)."/parse_ini_file.dat";

var_dump(parse_ini_file());
var_dump(parse_ini_file(1,1,1));
var_dump(parse_ini_file($filename));
var_dump(parse_ini_file($filename, true));

$ini = "
test =
";
file_put_contents($filename, $ini);
var_dump(parse_ini_file($filename));

$ini = "
test==
";
file_put_contents($filename, $ini);
var_dump(parse_ini_file($filename));

$ini = "
test=test=
";
file_put_contents($filename, $ini);
var_dump(parse_ini_file($filename));

$ini = "
test= \"new
line\"
";
file_put_contents($filename, $ini);
var_dump(parse_ini_file($filename));

define("TEST_CONST", "test const value");
$ini = "
test=TEST_CONST
";
file_put_contents($filename, $ini);
var_dump(parse_ini_file($filename));

$ini = "
[section]
test=hello
";
file_put_contents($filename, $ini);
var_dump(parse_ini_file($filename, true));

$ini = "
[section]
test=hello
";
file_put_contents($filename, $ini);
var_dump(parse_ini_file($filename, false));

$ini = "
section.test=hello
";
file_put_contents($filename, $ini);
var_dump(parse_ini_file($filename, true));

$ini = "
[section]
section.test=hello
";
file_put_contents($filename, $ini);
var_dump(parse_ini_file($filename, true));

$ini = "
[section]
1=2
";
file_put_contents($filename, $ini);
var_dump(parse_ini_file($filename, true));

$ini = "
1=2
";
file_put_contents($filename, $ini);
var_dump(parse_ini_file($filename, true));

$ini = "
test=test2
test=test3
test=test4
";
file_put_contents($filename, $ini);
var_dump(parse_ini_file($filename, true));


@unlink($filename);
echo "Done\n";
?>
--EXPECTF--	
Warning: Wrong parameter count for parse_ini_file() in %s on line %d
NULL

Warning: Wrong parameter count for parse_ini_file() in %s on line %d
NULL

Warning: parse_ini_file(%sparse_ini_file.dat): failed to open stream: No such file or directory in %s on line %d
bool(false)

Warning: parse_ini_file(%sparse_ini_file.dat): failed to open stream: No such file or directory in %s on line %d
bool(false)
array(1) {
  ["test"]=>
  string(0) ""
}

Warning: Error parsing %sparse_ini_file.dat on line 2
 in %s on line %d
bool(false)

Warning: Error parsing %sparse_ini_file.dat on line 2
 in %s on line %d
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
Done
