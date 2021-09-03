--TEST--
PDO SQLite open_basedir check
--EXTENSIONS--
pdo_sqlite
--INI--
open_basedir=.
--FILE--
<?php
chdir(__DIR__);

try {
    $db = new PDO('sqlite:../not_in_open_basedir.sqlite');
} catch (Exception $e) {
    echo $e->getMessage() . "\n";
}
try {
    $db = new PDO('sqlite:file:../not_in_open_basedir.sqlite');
} catch (Exception $e) {
    echo $e->getMessage() . "\n";
}
try {
    $db = new PDO('sqlite:file:../not_in_open_basedir.sqlite?mode=ro');
} catch (Exception $e) {
    echo $e->getMessage() . "\n";
}

?>
--EXPECT--
open_basedir prohibits opening ../not_in_open_basedir.sqlite
open_basedir prohibits opening file:../not_in_open_basedir.sqlite
open_basedir prohibits opening file:../not_in_open_basedir.sqlite?mode=ro
