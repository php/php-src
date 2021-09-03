--TEST--
Bug #39602 (Invalid session.save_handler crashes PHP)
--EXTENSIONS--
session
--INI--
session.save_handler=qwerty
error_reporting=0
--FILE--
<?php
ini_set("session.save_handler","files");
$x = new stdClass();
echo "ok";
?>
--EXPECT--
ok
