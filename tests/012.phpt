--TEST--
Check for Yaf_Route_Regex
--SKIPIF--
<?php if (!extension_loaded("yaf")) print "skip"; ?>
--FILE--
<?php 
$request = new Yaf_Request_Http("/subdir/ap/1.2/name/value", "/subdir");

$router = new Yaf_Router();

$route  = new Yaf_Route_regex(
	"#/subdir/(.*)#",
	array(
		"action" => "version",
	),
	array(
	)
);

$router->addRoute("subdir", $route)->addRoute("ap", new Yaf_Route_Regex(
	"#^/ap/([^/]*)/*#i",
    array(
		"action" => 'ap',
	),
	array(
		1 => 'version',
	)
))->route($request);

var_dump($router->getCurrentRoute());
var_dump($request->getParam('version'));
var_dump($request->getActionName());
var_dump($request->getControllerName());
var_dump($request->getParam('name'));

?>
--EXPECTF--
string(2) "ap"
string(3) "1.2"
string(2) "ap"
NULL
NULL
