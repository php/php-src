--TEST--
rfc1867 boundary 2
--INI--
post_max_size=1024
error_reporting=E_ALL&~E_NOTICE
comment=debug builds show some additional E_NOTICE errors
--POST_RAW--
Content-Type: multipart/form-data; boundary=------------------------------------foo, bar
--------------------------------------foo
Content-Disposition: form-data; name="foobar"

1
--------------------------------------foo--
--FILE--
<?php
var_dump($_FILES);
var_dump($_POST);
?>
--EXPECTF--
array(0) {
}
array(1) {
  [%u|b%"foobar"]=>
  %unicode|string%(1) "1"
}
