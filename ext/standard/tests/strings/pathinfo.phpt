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
  [u"basename"]=>
  unicode(0) ""
  [u"filename"]=>
  unicode(0) ""
}
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
  unicode(1) "."
  [u"basename"]=>
  unicode(2) ".."
  [u"extension"]=>
  unicode(0) ""
  [u"filename"]=>
  unicode(1) "."
}
array(3) {
  [u"dirname"]=>
  unicode(1) "%e"
  [u"basename"]=>
  unicode(0) ""
  [u"filename"]=>
  unicode(0) ""
}
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
  unicode(1) "%e"
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
unicode(12) "pathinfo.php"
unicode(8) "pathinfo"
unicode(3) "php"
unicode(%d) "%s%estrings"
unicode(%d) "%s%estrings"
unicode(12) "pathinfo.php"
unicode(3) "php"
unicode(12) "pathinfo.php"
unicode(%d) "%s%estrings"
unicode(12) "pathinfo.php"
unicode(%d) "%s%estrings"
unicode(%d) "%s%estrings"
Done
