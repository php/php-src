--TEST--
rfc1867 invalid boundary
--INI--
post_max_size=1024
--POST_RAW--
Content-Type: multipart/form-data; boundary="foobar
-----------------------------20896060251896012921717172737
Content-Disposition: form-data; name="foobar"

1
-----------------------------20896060251896012921717172737--
--FILE--
<?php
var_dump($_FILES);
var_dump($_POST);
?>
--EXPECTF--
Warning: PHP Request Startup: Invalid boundary in multipart/form-data POST data in %s
array(0) {
}
array(0) {
}
