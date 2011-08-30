--TEST--
Check for Yaf_Application
--SKIPIF--
<?php if (!extension_loaded("yaf")) print "skip"; ?>
--FILE--
<?php 
define("APPLICATION_PATH", dirname(__FILE__));
$app = new Yaf_Application(
	dirname(__FILE__) . "/simple.ini",
	'product');
print_r($app);
?>
--EXPECTF--
Yaf_Application Object
(
    [config:protected] => Yaf_Config_Ini Object
        (
            [_config:protected] => Array
                (
                    [application] => Array
                        (
                            [directory] => APPLICATION_PATCH/applcation
                        )

                    [name] => extra
                    [array] => Array
                        (
                            [1] => 1
                            [name] => new_name
                            [2] => test
                        )

                    [5] => 5
                    [routes] => Array
                        (
                            [regex] => Array
                                (
                                    [type] => regex
                                    [match] => ^/ap/(.*)
                                    [route] => Array
                                        (
                                            [controller] => Index
                                            [action] => action
                                        )

                                    [map] => Array
                                        (
                                            [0] => name
                                            [1] => name
                                            [2] => value
                                        )

                                )

                            [simple] => Array
                                (
                                    [type] => simple
                                    [controller] => c
                                    [module] => m
                                    [action] => a
                                )

                            [supervar] => Array
                                (
                                    [type] => supervar
                                    [varname] => c
                                )

                            [rewrite] => Array
                                (
                                    [type] => rewrite
                                    [match] => /ap/:name/:value
                                    [route] => Array
                                        (
                                            [controller] => Index
                                            [action] => action
                                        )

                                )

                        )

                    [value] => 2
                )

            [_readonly:protected] => 1
        )

    [dispatcher:protected] => Yaf_Dispatcher Object
        (
            [_router:protected] => Yaf_Router Object
                (
                    [_routes:protected] => Array
                        (
                            [_default] => Yaf_Route_Static Object
                                (
                                )

                        )

                    [_current:protected] => 
                )

            [_view:protected] => 
            [_request:protected] => Yaf_Request_Http Object
                (
                    [module] => 
                    [controller] => 
                    [action] => 
                    [method] => Cli
                    [params:protected] => Array
                        (
                        )

                    [language:protected] => 
                    [_exception:protected] => 
                    [_base_uri:protected] => 
                    [uri:protected] => 
                    [dispatched:protected] => 
                    [routed:protected] => 
                )

            [_plugins:protected] => Array
                (
                )

            [_auto_render:protected] => 1
            [_return_response:protected] => 
            [_instantly_flush:protected] => 
            [_default_module:protected] => Index
            [_default_controller:protected] => Index
            [_default_action:protected] => index
        )

    [_modules:protected] => Array
        (
            [0] => Index
        )

    [_running:protected] => 
    [_environ:protected] => product
)
