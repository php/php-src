--TEST--
Bug #61728 (PHP crash when calling ob_start in request_shutdown phase)
--EXTENSIONS--
session
--FILE--
<?php
function output_html($ext) {
    return strlen($ext);
}

function open ($save_path, $session_name) {
    return true;
}

function close() {
    return true;
}

function read ($id) {
    return '';
}

function write ($id, $sess_data) {
    ob_start("output_html");
    echo "laruence";
    ob_end_flush();
    return true;
}

function destroy ($id) {
    return true;
}

function gc ($maxlifetime) {
    return true;
}

session_set_save_handler ("open", "close", "read", "write", "destroy", "gc");
session_start();
?>
--EXPECT--
8
