--TEST--
rfc1867 missing boundary
--INI--
post_max_size=1024
error_reporting=E_ALL&~E_NOTICE
comment=debug builds show some additional E_NOTICE errors
--POST_RAW--
Content-Type: multipart/form-data
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
Warning: Missing boundary in multipart/form-data POST data in %s
array(0) {
}
array(0) {
}
