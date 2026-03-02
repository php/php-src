--TEST--
ReflectionExtension::getINIEntries()
--INI--
user_agent=php
--FILE--
<?php
$ext = new ReflectionExtension("standard");
$inis = $ext->getINIEntries();
var_dump($inis["user_agent"]);
?>
--EXPECT--
string(3) "php"
