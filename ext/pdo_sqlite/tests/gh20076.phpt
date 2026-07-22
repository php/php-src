--TEST--
GH-20076 (open_basedir prohibits opening :memory:?cache=shared)
--EXTENSIONS--
sqlite3
--INI--
open_basedir=xx
--FILE--
<?php
var_dump(new PDO("sqlite::memory:?cache=shared"));
?>
--EXPECT--
object(PDO)#1 (0) {
}
