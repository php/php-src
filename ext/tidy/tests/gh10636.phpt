--TEST--
GH-10636 (Tidy does not output notice when it encountered parse errors in the default configuration file)
--EXTENSIONS--
tidy
--INI--
tidy.default_config={PWD}/gh10636_config
--FILE--
<?php
$a = new tidy(__DIR__."/007.html");
?>
--EXPECTF--
Notice: main(): There were errors while parsing the Tidy configuration file "%sgh10636_config" in %s on line %d
