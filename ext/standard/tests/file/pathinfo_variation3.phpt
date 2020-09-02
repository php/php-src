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
var_dump(pathinfo($testfile, PATHINFO_EXTENSION|PATHINFO_FILENAME|PATHINFO_DIRNAME));
var_dump(pathinfo($testfile, PATHINFO_EXTENSION|PATHINFO_FILENAME|PATHINFO_BASENAME));
var_dump(pathinfo($testfile, PATHINFO_EXTENSION|PATHINFO_FILENAME));
var_dump(pathinfo($testfile, PATHINFO_EXTENSION|PATHINFO_BASENAME));
var_dump(pathinfo($testfile, PATHINFO_FILENAME|PATHINFO_DIRNAME));
var_dump(pathinfo($testfile, PATHINFO_FILENAME|PATHINFO_BASENAME));
var_dump(pathinfo($testfile, PATHINFO_DIRNAME|PATHINFO_EXTENSION));
var_dump(pathinfo($testfile, PATHINFO_DIRNAME|PATHINFO_BASENAME));


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
string(17) "/usr/include/arpa"
string(6) "inet.h"
string(1) "h"
string(6) "inet.h"
string(17) "/usr/include/arpa"
string(6) "inet.h"
string(17) "/usr/include/arpa"
string(17) "/usr/include/arpa"
