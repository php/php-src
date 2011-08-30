--TEST--
Check for Yaf_Loader
--SKIPIF--
<?php if (!extension_loaded("yaf")) print "skip"; ?>
--FILE--
<?php 
ini_set("ap.lowcase_path", FALSE);
$loader = Yaf_Loader::getInstance(dirname(__FILE__), dirname(__FILE__) . "/global");
$loader->registerLocalNamespace("Baidu");
var_dump($loader->isLocalName("Baidu_Name"));

try {
	var_dump($loader->autoload("Baidu_Name"));
} catch (Yaf_Exception_LoadFailed $e) {
	var_dump($e->getMessage());
} 
try {
	var_dump($loader->autoload("Global_Name"));
} catch (Yaf_Exception_LoadFailed $e) {
	var_dump($e->getMessage());
} 

?>
--EXPECTF--
bool(true)

Warning: Yaf_Loader::autoload(): Could not find script %s/Baidu/Name.php in %s
bool(true)

Warning: Yaf_Loader::autoload(): Could not find script %s/global/Global/Name.php in %s
bool(true)
