--TEST--
Check for Yaf_Route_Static
--SKIPIF--
<?php if (!extension_loaded("yaf")) print "skip"; ?>
--FILE--
<?php 
$request_uri = "/prefix/controller/action/name/laruence/age/28";
$base_uri	 = "/prefix/";

$request = new Yaf_Request_Http($request_uri, $base_uri);

unset($base_uri);
unset($request_uri);

$route = new Yaf_Route_Static();

var_dump($route->route($request));

print_r($request);
?>
--EXPECTF--
bool(true)
Yaf_Request_Http Object
(
    [module] => 
    [controller] => controller
    [action] => action
    [method] => Cli
    [params:protected] => Array
        (
            [name] => laruence
            [age] => 28
        )

    [language:protected] => 
    [_exception:protected] => 
    [_base_uri:protected] => /prefix/
    [uri:protected] => /prefix/controller/action/name/laruence/age/28
    [dispatched:protected] => 
    [routed:protected] => 
)
