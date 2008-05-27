--TEST--
ReflectionExtension::getINIEntries()
--SKIPIF--
<?php extension_loaded('reflection') or die('skip'); ?>
--INI--
user_agent=php
--FILE--
<?php
$ext = new ReflectionExtension("standard");
$inis = $ext->getINIEntries();
var_dump($inis["user_agent"]);
?>
--EXPECT--	
unicode(3) "php"

