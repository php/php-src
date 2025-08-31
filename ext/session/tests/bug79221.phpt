--TEST--
Null Pointer Dereference in PHP Session Upload Progress
--EXTENSIONS--
session
--INI--
error_reporting=0
file_uploads=1
upload_max_filesize=1024
session.save_path=
session.name=PHPSESSID
session.serialize_handler=php
session.use_strict_mode=0
session.use_cookies=1
session.use_only_cookies=0
session.upload_progress.enabled=1
session.upload_progress.cleanup=0
session.upload_progress.prefix=upload_progress_
session.upload_progress.name=PHP_SESSION_UPLOAD_PROGRESS
session.upload_progress.freq=1%
session.upload_progress.min_freq=0.000000001
--COOKIE--
PHPSESSID=session-upload
--POST_RAW--
Content-Type: multipart/form-data; boundary=---------------------------20896060251896012921717172737
-----------------------------20896060251896012921717172737
Content-Disposition: form-data; name="PHPSESSID"

session-upload
-----------------------------20896060251896012921717172737
Content-Disposition: form-data; name="PHP_SESSION_UPLOAD_PROGRESS"

ryat
-----------------------------20896060251896012921717172737
Content-Disposition: form-data; file="file"; ryat="filename"

1
-----------------------------20896060251896012921717172737--
--FILE--
<?php

session_start();
var_dump($_SESSION);
session_destroy();

?>
--EXPECT--
array(0) {
}
