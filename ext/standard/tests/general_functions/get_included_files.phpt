--TEST--
Test get_include_files() function
--INI--
include_path=.
--FILE--
<?php


echo "*** Testing get_included_files()\n";

echo "\n-- List included files at start --\n";
var_dump(get_included_files());

include(__DIR__."/get_included_files_inc1.inc");
echo "\n-- List included files atfter including inc1 -\n";
var_dump(get_included_files());

include(__DIR__."/get_included_files_inc2.inc");
echo "\n-- List included files atfter including inc2 which will include inc3 which includes inc1 --\n";
var_dump(get_included_files());

?>
--EXPECTF--
*** Testing get_included_files()

-- List included files at start --
array(1) {
  [0]=>
  string(%d) "%sget_included_files.php"
}

-- List included files atfter including inc1 -
array(2) {
  [0]=>
  string(%d) "%sget_included_files.php"
  [1]=>
  string(%d) "%sget_included_files_inc1.inc"
}

-- List included files atfter including inc2 which will include inc3 which includes inc1 --
array(4) {
  [0]=>
  string(%d) "%sget_included_files.php"
  [1]=>
  string(%d) "%sget_included_files_inc1.inc"
  [2]=>
  string(%d) "%sget_included_files_inc2.inc"
  [3]=>
  string(%d) "%sget_included_files_inc3.inc"
}
