--TEST--
Bug #80889 (Cannot set save handler when save_handler is invalid)
--EXTENSIONS--
session
--INI--
session.save_handler=whatever
--FILE--
<?php
$initHandler = ini_get('session.save_handler');
session_set_save_handler(
    function ($savePath, $sessionName) {
        return true;
    },
    function () {
        return true;
    },
    function ($id) {
        return '';
    },
    function ($id, $data) {
        return true;
    },
    function ($id) {
        return true;
    },
    function ($maxlifetime) {
        return true;
    }
);
$setHandler = ini_get('session.save_handler');
var_dump($initHandler, $setHandler);
?>
--EXPECTF--
Deprecated: Calling session_set_save_handler() with more than 2 arguments is deprecated in %s on line %d

Warning: session_set_save_handler(): Session save handler cannot be changed after headers have already been sent in %s on line %d
string(8) "whatever"
string(8) "whatever"
