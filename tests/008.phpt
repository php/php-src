--TEST--
Check for Yaf_Router
--SKIPIF--
<?php if (!extension_loaded("yaf")) print "skip"; ?>
--FILE--
<?php

$router = new Yaf_Router();

$route  = new Yaf_Route_Simple('m', 'c', 'a');
$sroute = new Yaf_Route_Supervar('r');

$router->addRoute("simple", $route)->addRoute("super", $sroute);
print_r($router);
var_dump($router->getCurrentRoute());
print_r($router->getRoutes());
print_r($router->getRoute("simple"));
var_dump($router->getRoute("noexists"));
--EXPECTF--
Yaf_Router Object
(
    [_routes:protected] => Array
        (
            [_default] => Yaf_Route_Static Object
                (
                )

            [simple] => Yaf_Route_Simple Object
                (
                    [controller:protected] => c
                    [module:protected] => m
                    [action:protected] => a
                )

            [super] => Yaf_Route_Supervar Object
                (
                    [_var_name:protected] => r
                )

        )

    [_current:protected] => 
)
NULL
Array
(
    [_default] => Yaf_Route_Static Object
        (
        )

    [simple] => Yaf_Route_Simple Object
        (
            [controller:protected] => c
            [module:protected] => m
            [action:protected] => a
        )

    [super] => Yaf_Route_Supervar Object
        (
            [_var_name:protected] => r
        )

)
Yaf_Route_Simple Object
(
    [controller:protected] => c
    [module:protected] => m
    [action:protected] => a
)
NULL
