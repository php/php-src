--TEST--
import_request_variables() test (overwrite super-globals)
--SKIPIF--
<?php if(PHP_VERSION_ID >= 50399){ die('skip not needed anymore without register_globals'); } ?>
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
  ["GET"]=>
  string(1) "0"
  ["POST"]=>
  string(1) "1"
  ["COOKIE"]=>
  string(1) "2"
  ["FILES"]=>
  string(1) "3"
  ["REQUEST"]=>
  string(1) "4"
}
array(5) {
  ["GET"]=>
  string(1) "5"
  ["POST"]=>
  string(1) "6"
  ["COOKIE"]=>
  string(1) "7"
  ["FILES"]=>
  string(1) "8"
  ["REQUEST"]=>
  string(1) "9"
}
array(5) {
  ["GET"]=>
  string(2) "10"
  ["POST"]=>
  string(2) "11"
  ["COOKIE"]=>
  string(2) "12"
  ["FILES"]=>
  string(2) "13"
  ["REQUEST"]=>
  string(2) "14"
}
array(0) {
}
array(5) {
  ["GET"]=>
  string(1) "5"
  ["POST"]=>
  string(1) "6"
  ["COOKIE"]=>
  string(1) "7"
  ["FILES"]=>
  string(1) "8"
  ["REQUEST"]=>
  string(1) "9"
}
Done
