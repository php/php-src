--TEST--
Test pathinfo() function : usage variation 
--CREDITS--
Dave Kelsey <d_kelsey@uk.ibm.com>
--FILE--
<?php
/* Prototype  : array pathinfo(string path[, int options])
 * Description: Returns information about a certain string 
 * Source code: ext/standard/string.c
 * Alias to functions: 
 */

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
===DONE===
--EXPECTF--
*** Testing pathinfo() : usage variation ***
array(4) {
  [u"dirname"]=>
  unicode(1) "."
  [u"basename"]=>
  unicode(1) "."
  [u"extension"]=>
  unicode(0) ""
  [u"filename"]=>
  unicode(0) ""
}
array(4) {
  [u"dirname"]=>
  unicode(1) "%s"
  [u"basename"]=>
  unicode(1) "."
  [u"extension"]=>
  unicode(0) ""
  [u"filename"]=>
  unicode(0) ""
}
array(4) {
  [u"dirname"]=>
  unicode(1) "."
  [u"basename"]=>
  unicode(10) ".cvsignore"
  [u"extension"]=>
  unicode(9) "cvsignore"
  [u"filename"]=>
  unicode(0) ""
}
unicode(6) "inet.h"
unicode(4) "inet"
unicode(1) "h"
unicode(17) "/usr/include/arpa"
unicode(17) "/usr/include/arpa"
unicode(6) "inet.h"
unicode(1) "h"
unicode(6) "inet.h"
unicode(17) "/usr/include/arpa"
unicode(6) "inet.h"
unicode(17) "/usr/include/arpa"
unicode(17) "/usr/include/arpa"
===DONE===

