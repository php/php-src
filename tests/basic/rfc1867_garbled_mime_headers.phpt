--TEST--
rfc1867 garbled mime headers
--INI--
file_uploads=1
upload_max_filesize=1024
--POST_RAW--
Content-Type: multipart/form-data; boundary=---------------------------20896060251896012921717172737
-----------------------------20896060251896012921717172737
Content-Disposition: form-data


-----------------------------20896060251896012921717172737--
--FILE--
<?php
var_dump($_FILES);
var_dump($_POST);
?>
--EXPECTF--
Warning: PHP Request Startup: File Upload Mime headers garbled in %s
array(0) {
}
array(0) {
}
