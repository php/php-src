--TEST--
pathinfo() tests
--FILE--
<?php

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

try {
	pathinfo(__FILE__, PATHINFO_EXTENSION|PATHINFO_FILENAME|PATHINFO_DIRNAME);
} catch (\ValueError $e) {
	echo $e->getMessage(), PHP_EOL;
}
try {
	pathinfo(__FILE__, PATHINFO_EXTENSION|PATHINFO_FILENAME);
} catch (\ValueError $e) {
	echo $e->getMessage(), PHP_EOL;
}
try {
	pathinfo(__FILE__, PATHINFO_EXTENSION|PATHINFO_DIRNAME);
} catch (\ValueError $e) {
	echo $e->getMessage(), PHP_EOL;
}
try {
	pathinfo(__FILE__, PATHINFO_FILENAME|PATHINFO_BASENAME);
} catch (\ValueError $e) {
	echo $e->getMessage(), PHP_EOL;
}
try {
	pathinfo(__FILE__, PATHINFO_DIRNAME|PATHINFO_EXTENSION);
} catch (\ValueError $e) {
	echo $e->getMessage(), PHP_EOL;
}
try {
	pathinfo(__FILE__, PATHINFO_DIRNAME|PATHINFO_BASENAME);
} catch (\ValueError $e) {
	echo $e->getMessage(), PHP_EOL;
}

try {
	pathinfo(__FILE__, PATHINFO_DIRNAME-1);
} catch (\ValueError $e) {
	echo $e->getMessage(), PHP_EOL;
}
try {
	pathinfo(__FILE__, PATHINFO_ALL+1);
} catch (\ValueError $e) {
	echo $e->getMessage(), PHP_EOL;
}

echo "Done\n";
?>
--EXPECTF--
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
pathinfo(): Argument #2 ($flags) must be only one of the PATHINFO_* constants
pathinfo(): Argument #2 ($flags) must be only one of the PATHINFO_* constants
pathinfo(): Argument #2 ($flags) must be only one of the PATHINFO_* constants
pathinfo(): Argument #2 ($flags) must be only one of the PATHINFO_* constants
pathinfo(): Argument #2 ($flags) must be only one of the PATHINFO_* constants
pathinfo(): Argument #2 ($flags) must be only one of the PATHINFO_* constants
pathinfo(): Argument #2 ($flags) must be one of the PATHINFO_* constants
pathinfo(): Argument #2 ($flags) must be one of the PATHINFO_* constants
Done
