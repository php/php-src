--TEST--
rfc1867 file_upload disabled
--INI--
file_uploads=0
error_reporting=E_ALL&~E_NOTICE
comment=debug builds show some additional E_NOTICE errors
--POST_RAW--
Content-Type: multipart/form-data; boundary=---------------------------20896060251896012921717172737
-----------------------------20896060251896012921717172737
Content-Disposition: form-data; name="foo"

bar
-----------------------------20896060251896012921717172737
Content-Disposition: form-data; name="file1"; filename="file1.txt"
Content-Type: text/plain-file1

1
-----------------------------20896060251896012921717172737
Content-Disposition: form-data; name="bar"

foo
-----------------------------20896060251896012921717172737--
--FILE--
<?php
var_dump($_FILES);
var_dump($_POST);
?>
--EXPECTF--
array(0) {
}
array(2) {
  [%u|b%"foo"]=>
  %unicode|string%(3) "bar"
  [%u|b%"bar"]=>
  %unicode|string%(3) "foo"
}
