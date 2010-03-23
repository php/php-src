--TEST--
parse_ini_file() multiple calls
--SKIPIF--
<?php if( substr(PHP_OS, 0, 3) == 'WIN' ) die("skip Windows has different error message");?>
--FILE--
<?php

$filename = dirname(__FILE__)."/parse_ini_file.dat";
@unlink($filename); /* Make sure the file really does not exist! */

var_dump(parse_ini_file());
var_dump(parse_ini_file(1,1,1,1));
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

/* From bug #44574 */
$ini = "[section1]\nname = value";
file_put_contents($filename, $ini);
var_dump(parse_ini_file($filename, true));

/* #44842, labels starting with underscore */
$ini = <<<'INI'
foo=bar1
_foo=bar2
foo_=bar3
INI;
file_put_contents($filename, $ini);
var_dump(parse_ini_file($filename, true));

/* #44575, comments starting with '#' */
$ini = <<<'INI'
foo=bar1
; comment
_foo=bar2
# comment
foo_=bar3
INI;
file_put_contents($filename, $ini);
var_dump(parse_ini_file($filename, true));


@unlink($filename);
echo "Done\n";
?>
--EXPECTF--	
Warning: parse_ini_file() expects at least 1 parameter, 0 given in %sparse_ini_file.php on line 6
bool(false)

Warning: parse_ini_file() expects at most 3 parameters, 4 given in %sparse_ini_file.php on line 7
bool(false)

Warning: parse_ini_file(%sparse_ini_file.dat): failed to open stream: No such file or directory in %sparse_ini_file.php on line 8
bool(false)

Warning: parse_ini_file(%sparse_ini_file.dat): failed to open stream: No such file or directory in %sparse_ini_file.php on line 9
bool(false)
array(1) {
  ["test"]=>
  string(0) ""
}

Warning: syntax error, unexpected '=' in %sparse_ini_file.dat on line 2
 in %sparse_ini_file.php on line 20
bool(false)

Warning: syntax error, unexpected '=' in %sparse_ini_file.dat on line 2
 in %sparse_ini_file.php on line 26
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

Deprecated: Comments starting with '#' are deprecated in %s
array(3) {
  ["foo"]=>
  string(4) "bar1"
  ["_foo"]=>
  string(4) "bar2"
  ["foo_"]=>
  string(4) "bar3"
}
Done
