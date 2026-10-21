--TEST--
Session upload progress is disabled even when enabled in INI
--SKIPIF--
<?php include('skipif.inc'); ?>
--INI--
file_uploads=1
session.save_path=
session.upload_progress.enabled=1
session.upload_progress.cleanup=0
session.upload_progress.prefix=upload_progress_
session.upload_progress.name=PHP_SESSION_UPLOAD_PROGRESS
--COOKIE--
PHPSESSID=php71-cve2026109619
--POST_RAW--
Content-Type: multipart/form-data; boundary=php71-security
--php71-security
Content-Disposition: form-data; name="PHP_SESSION_UPLOAD_PROGRESS"

security-test
--php71-security
Content-Disposition: form-data; name="upload"; filename="test.txt"

hello
--php71-security--
--FILE--
<?php
session_start();
var_dump(isset($_SESSION['upload_progress_security-test']));
var_dump($_FILES['upload']['error'], $_FILES['upload']['size']);
session_destroy();
?>
--EXPECT--
bool(false)
int(0)
int(5)
