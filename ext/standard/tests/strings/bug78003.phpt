--TEST--
Bug #78003 (strip_tags output change since PHP 7.3)
--FILE--
<?php
var_dump(
    strip_tags('<foo<>bar>'),
    strip_tags('<foo<!>bar>'),
    strip_tags('<foo<?>bar>')
);
?>
===DONE===
--EXPECT--
string(0) ""
string(0) ""
string(0) ""
===DONE===
