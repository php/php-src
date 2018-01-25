--TEST--
pathinfo() tests
--FILE--
<?php

var_dump(pathinfo());
var_dump(pathinfo(""));
var_dump(pathinfo("."));
var_dump(pathinfo(".."));
var_dump(pathinfo("/"));
var_dump(pathinfo("./"));
var_dump(pathinfo("/."));
var_dump(pathinfo(".cvsignore"));
var_dump(pathinfo(__FILE__, PATHINFO_BASENAME));
var_dump(pathinfo(__FILE__, PATHINFO_FILENAME));
var_dump(pathinfo(__FILE__, PATHINFO_EXTENSION));
var_dump(pathinfo(__FILE__, PATHINFO_DIRNAME));
var_dump(pathinfo(__FILE__, PATHINFO_EXTENSION|PATHINFO_FILENAME|PATHINFO_DIRNAME));
var_dump(pathinfo(__FILE__, PATHINFO_EXTENSION|PATHINFO_FILENAME|PATHINFO_BASENAME));
var_dump(pathinfo(__FILE__, PATHINFO_EXTENSION|PATHINFO_FILENAME));
var_dump(pathinfo(__FILE__, PATHINFO_EXTENSION|PATHINFO_BASENAME));
var_dump(pathinfo(__FILE__, PATHINFO_FILENAME|PATHINFO_DIRNAME));
var_dump(pathinfo(__FILE__, PATHINFO_FILENAME|PATHINFO_BASENAME));
var_dump(pathinfo(__FILE__, PATHINFO_DIRNAME|PATHINFO_EXTENSION));
var_dump(pathinfo(__FILE__, PATHINFO_DIRNAME|PATHINFO_BASENAME));

echo "Done\n";
?>
--EXPECTF--
Warning: pathinfo() expects at least 1 parameter, 0 given in %s on line %d
NULL
array(2) {
  ["basename"]=>
  string(0) ""
  ["filename"]=>
  string(0) ""
}
array(4) {
  ["dirname"]=>
  string(1) "."
  ["basename"]=>
  string(1) "."
  ["extension"]=>
  string(0) ""
  ["filename"]=>
  string(0) ""
}
array(4) {
  ["dirname"]=>
  string(1) "."
  ["basename"]=>
  string(2) ".."
  ["extension"]=>
  string(0) ""
  ["filename"]=>
  string(1) "."
}
array(3) {
  ["dirname"]=>
  string(1) "%e"
  ["basename"]=>
  string(0) ""
  ["filename"]=>
  string(0) ""
}
array(4) {
  ["dirname"]=>
  string(1) "."
  ["basename"]=>
  string(1) "."
  ["extension"]=>
  string(0) ""
  ["filename"]=>
  string(0) ""
}
array(4) {
  ["dirname"]=>
  string(1) "%e"
  ["basename"]=>
  string(1) "."
  ["extension"]=>
  string(0) ""
  ["filename"]=>
  string(0) ""
}
array(4) {
  ["dirname"]=>
  string(1) "."
  ["basename"]=>
  string(10) ".cvsignore"
  ["extension"]=>
  string(9) "cvsignore"
  ["filename"]=>
  string(0) ""
}
string(12) "pathinfo.php"
string(8) "pathinfo"
string(3) "php"
string(%d) "%s%estrings"
string(%d) "%s%estrings"
string(12) "pathinfo.php"
string(3) "php"
string(12) "pathinfo.php"
string(%d) "%s%estrings"
string(12) "pathinfo.php"
string(%d) "%s%estrings"
string(%d) "%s%estrings"
Done
