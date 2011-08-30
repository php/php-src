--TEST--
Check for Yaf_View_Simple
--SKIPIF--
<?php if (!extension_loaded("yaf")) print "skip"; ?>
--FILE--
<?php
$view = new Yaf_View_Simple(dirname(__FILE__));
$value = "laruence";
$view->assign("name", $value);
unset($value);
print_r($view);
var_dump(strlen($view->render(dirname(__FILE__) . "/002.phpt")));
var_dump($view->name);
var_dump($view->noexists);
print_r($view);
--EXPECTF--
Yaf_View_Simple Object
(
    [_tpl_vars:protected] => Array
        (
            [name] => laruence
        )

    [_tpl_dir:protected] => %s
)
int(1590)
string(8) "laruence"
NULL
Yaf_View_Simple Object
(
    [_tpl_vars:protected] => Array
        (
            [name] => laruence
        )

    [_tpl_dir:protected] => %s
)
