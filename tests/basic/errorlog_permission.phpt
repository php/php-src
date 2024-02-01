--TEST--
Check permissions for created errorlog file
--SKIPIF--
<?php 
if (strtoupper(substr(PHP_OS, 0, 3)) === 'WIN') {
    die("skip this test on windows");
}
?>
--INI--
error_log=error_permissions_test.log
error_log_mode=0600
--FILE--
<?php

const LOG_FILENAME='error_permissions_test.log';

try {
    if (file_exists(LOG_FILENAME)) {
        unlink(LOG_FILENAME);
    }
    $oldMask = umask(0000);

    error_log("hello world");

    assert(file_exists(LOG_FILENAME));

    printf("got permissions=%o\n", fileperms(LOG_FILENAME) & 0777);
    printf("errorlog contents\n%s", file_get_contents(LOG_FILENAME));
    
    umask($oldMask);
} finally {
    unlink(LOG_FILENAME);
}
?>
--EXPECTF--
got permissions=600
errorlog contents
[%d-%s-%d %d:%d:%d %s] hello world
