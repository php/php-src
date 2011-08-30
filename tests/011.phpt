--TEST--
Check for Yaf_Route_Rewrite
--SKIPIF--
<?php if (!extension_loaded("yaf")) print "skip"; ?>
--FILE--
<?php 
$request = new Yaf_Request_Http("/subdir/ap/1.2/name/value", "/subdir");

$router = new Yaf_Router();

$route  = new Yaf_Route_Rewrite(
	"/subdir/:name/:version",
	array(
		"action" => "version",
	)
);

$router->addRoute("subdir", $route)->addRoute("ap", new Yaf_Route_Rewrite(
	"/ap/:version/*",
    array(
		"action" => 'ap',
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
string(5) "value"
