--TEST--
GH-25307 (NULL ps_files dereference in ps_files_close at shutdown)
--EXTENSIONS--
session
--SKIPIF--
<?php include(__DIR__ . '/../skipif.inc'); ?>
--CREDITS--
YuanchengJiang
--FILE--
<?php
ob_start();
session_set_save_handler(new SessionHandler(), true);
ini_set("session.save_handler", "files");
session_start();
try {
    (new SessionHandler)->open("", "");
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
echo "Done\n";
?>
--EXPECT--
Cannot call default session handler
Done
