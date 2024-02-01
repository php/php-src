--TEST--
PDO_sqlite: Testing URIs with open_basedir
--EXTENSIONS--
pdo_sqlite
--INI--
open_basedir="{TMP}"
--FILE--
<?php

// create in basedir
$filename = sys_get_temp_dir() . DIRECTORY_SEPARATOR . 'pdo_sqlite_filename.db';

new PDO('sqlite:file:' . $filename);
?>

--CLEAN--
<?php
$filenames = [
    sys_get_temp_dir() . DIRECTORY_SEPARATOR . 'pdo_sqlite_filename.db',
];
foreach ($filenames as $filename) {
    if (file_exists($filename)) {
        unlink($filename);
    }
}
?>
--EXPECTF--
Fatal error: Uncaught PDOException: open_basedir prohibits opening %s in %s:%d
Stack trace:
%s
#1 {main}
  thrown in %s
