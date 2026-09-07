--TEST--
Test pathinfo() function : usage variation
--CREDITS--
Dave Kelsey <d_kelsey@uk.ibm.com>
--FILE--
<?php
echo "*** Testing pathinfo() : usage variation ***\n";

$testfile = "/usr/include/arpa/inet.h";

var_dump(pathinfo("./"));
var_dump(pathinfo("/."));
var_dump(pathinfo(".cvsignore"));
var_dump(pathinfo($testfile, PATHINFO_BASENAME));
var_dump(pathinfo($testfile, PATHINFO_FILENAME));
var_dump(pathinfo($testfile, PATHINFO_EXTENSION));
var_dump(pathinfo($testfile, PATHINFO_DIRNAME));

try {
	pathinfo($testfile, PATHINFO_EXTENSION|PATHINFO_FILENAME|PATHINFO_DIRNAME);
} catch (\ValueError $e) {
	echo $e->getMessage(), PHP_EOL;
}
try {
	pathinfo($testfile, PATHINFO_EXTENSION|PATHINFO_FILENAME);
} catch (\ValueError $e) {
	echo $e->getMessage(), PHP_EOL;
}
try {
	pathinfo($testfile, PATHINFO_EXTENSION|PATHINFO_DIRNAME);
} catch (\ValueError $e) {
	echo $e->getMessage(), PHP_EOL;
}
try {
	pathinfo($testfile, PATHINFO_FILENAME|PATHINFO_BASENAME);
} catch (\ValueError $e) {
	echo $e->getMessage(), PHP_EOL;
}
try {
	pathinfo($testfile, PATHINFO_DIRNAME|PATHINFO_EXTENSION);
} catch (\ValueError $e) {
	echo $e->getMessage(), PHP_EOL;
}
try {
	pathinfo($testfile, PATHINFO_DIRNAME|PATHINFO_BASENAME);
} catch (\ValueError $e) {
	echo $e->getMessage(), PHP_EOL;
}

?>
--EXPECTF--
*** Testing pathinfo() : usage variation ***
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
  string(1) "%s"
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
string(6) "inet.h"
string(4) "inet"
string(1) "h"
string(17) "/usr/include/arpa"
pathinfo(): Argument #2 ($flags) must be only one of the PATHINFO_* constants
pathinfo(): Argument #2 ($flags) must be only one of the PATHINFO_* constants
pathinfo(): Argument #2 ($flags) must be only one of the PATHINFO_* constants
pathinfo(): Argument #2 ($flags) must be only one of the PATHINFO_* constants
pathinfo(): Argument #2 ($flags) must be only one of the PATHINFO_* constants
pathinfo(): Argument #2 ($flags) must be only one of the PATHINFO_* constants
