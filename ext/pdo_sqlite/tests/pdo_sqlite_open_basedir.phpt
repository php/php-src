--TEST--
PDO_sqlite: Testing filenames with open_basedir
--EXTENSIONS--
pdo_sqlite
--INI--
open_basedir=.
--FILE--
<?php

// create in basedir
$filename = 'pdo_sqlite_filename.db';

$db = new PDO('sqlite:' . $filename);

var_dump($db->exec('CREATE TABLE test1 (id INT);'));

// create outside basedir
$filename = '..' . DIRECTORY_SEPARATOR . 'pdo_sqlite_filename.db';

new PDO('sqlite:' . $filename);
?>

--CLEAN--
<?php
$filenames = [
    'pdo_sqlite_filename.db',
    '..' . DIRECTORY_SEPARATOR . 'pdo_sqlite_filename.db',
];
foreach ($filenames as $filename) {
    if (file_exists($filename)) {
        unlink($filename);
    }
}
?>
--EXPECTF--
int(0)

Fatal error: Uncaught PDOException: PDO::__construct(): open_basedir restriction in effect. File(%s) is not within the allowed path(s): (%s) in %s:%d
Stack trace:
%s
#1 {main}

Next PDOException: open_basedir prohibits opening %s in %s:%d
Stack trace:
%s
#1 {main}
  thrown in %s
