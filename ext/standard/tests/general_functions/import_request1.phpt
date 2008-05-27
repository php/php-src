--TEST--
import_request_variables() test (overwrite super-globals)
--GET--
GET=0&POST=1&COOKIE=2&FILES=3&REQUEST=4
--POST--
GET=5&POST=6&COOKIE=7&FILES=8&REQUEST=9
--COOKIE--
GET=10;POST=11;COOKIE=12;FILES=13;REQUEST=14
--INI--
variables_order=CGP
--FILE--
<?php

import_request_variables("gpc", "_");
var_dump($_GET, $_POST, $_COOKIE, $_FILES, $_REQUEST);

echo "Done\n";
?>
--EXPECTF--
Warning: import_request_variables(): Attempted super-global (_GET) variable overwrite in %s on line %d

Warning: import_request_variables(): Attempted super-global (_POST) variable overwrite in %s on line %d

Warning: import_request_variables(): Attempted super-global (_COOKIE) variable overwrite in %s on line %d

Warning: import_request_variables(): Attempted super-global (_FILES) variable overwrite in %s on line %d

Warning: import_request_variables(): Attempted super-global (_REQUEST) variable overwrite in %s on line %d

Warning: import_request_variables(): Attempted super-global (_GET) variable overwrite in %s on line %d

Warning: import_request_variables(): Attempted super-global (_POST) variable overwrite in %s on line %d

Warning: import_request_variables(): Attempted super-global (_COOKIE) variable overwrite in %s on line %d

Warning: import_request_variables(): Attempted super-global (_FILES) variable overwrite in %s on line %d

Warning: import_request_variables(): Attempted super-global (_REQUEST) variable overwrite in %s on line %d

Warning: import_request_variables(): Attempted super-global (_GET) variable overwrite in %s on line %d

Warning: import_request_variables(): Attempted super-global (_POST) variable overwrite in %s on line %d

Warning: import_request_variables(): Attempted super-global (_COOKIE) variable overwrite in %s on line %d

Warning: import_request_variables(): Attempted super-global (_FILES) variable overwrite in %s on line %d

Warning: import_request_variables(): Attempted super-global (_REQUEST) variable overwrite in %s on line %d
array(5) {
  [u"GET"]=>
  unicode(1) "0"
  [u"POST"]=>
  unicode(1) "1"
  [u"COOKIE"]=>
  unicode(1) "2"
  [u"FILES"]=>
  unicode(1) "3"
  [u"REQUEST"]=>
  unicode(1) "4"
}
array(5) {
  [u"GET"]=>
  unicode(1) "5"
  [u"POST"]=>
  unicode(1) "6"
  [u"COOKIE"]=>
  unicode(1) "7"
  [u"FILES"]=>
  unicode(1) "8"
  [u"REQUEST"]=>
  unicode(1) "9"
}
array(5) {
  [u"GET"]=>
  unicode(2) "10"
  [u"POST"]=>
  unicode(2) "11"
  [u"COOKIE"]=>
  unicode(2) "12"
  [u"FILES"]=>
  unicode(2) "13"
  [u"REQUEST"]=>
  unicode(2) "14"
}
array(0) {
}
array(5) {
  [u"GET"]=>
  unicode(1) "5"
  [u"POST"]=>
  unicode(1) "6"
  [u"COOKIE"]=>
  unicode(1) "7"
  [u"FILES"]=>
  unicode(1) "8"
  [u"REQUEST"]=>
  unicode(1) "9"
}
Done
