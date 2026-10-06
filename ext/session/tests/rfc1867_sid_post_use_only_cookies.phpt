--TEST--
session rfc1867 upload progress does not use form SID when use_only_cookies=1
--INI--
file_uploads=1
upload_max_filesize=1024
session.save_path=
session.name=PHPSESSID
session.use_strict_mode=0
session.use_cookies=1
session.use_only_cookies=1
session.upload_progress.enabled=1
session.upload_progress.cleanup=0
session.upload_progress.prefix=upload_progress_
session.upload_progress.name=PHP_SESSION_UPLOAD_PROGRESS
session.upload_progress.freq=0
session.save_handler=files
--EXTENSIONS--
session
--SKIPIF--
<?php include('skipif.inc'); ?>
--POST_RAW--
Content-Type: multipart/form-data; boundary=---------------------------20896060251896012921717172737
-----------------------------20896060251896012921717172737
Content-Disposition: form-data; name="PHPSESSID"

rfc1867-sid-post-use-only-cookies
-----------------------------20896060251896012921717172737
Content-Disposition: form-data; name="PHP_SESSION_UPLOAD_PROGRESS"

rfc1867_sid_post_use_only_cookies.php
-----------------------------20896060251896012921717172737
Content-Disposition: form-data; name="file1"; filename="file1.txt"

1
-----------------------------20896060251896012921717172737--
--FILE--
<?php
session_id("rfc1867-sid-post-use-only-cookies");
session_start();
var_dump(isset($_SESSION["upload_progress_" . basename(__FILE__)]));
session_destroy();
?>
--EXPECT--
bool(false)
