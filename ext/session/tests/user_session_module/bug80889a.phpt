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
--EXPECT--
string(8) "whatever"
string(4) "user"
