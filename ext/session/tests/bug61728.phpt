--TEST--
Bug #61728 (php-fpm SIGSEGV running friendica on nginx)
--SKIPIF--
<?php include('skipif.inc'); ?>
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
} 

function write ($id, $sess_data) { 
    ob_start("output_html");
    echo "laruence";
    ob_end_flush();
    return true;
} 

function destroy ($id) { 
} 

function gc ($maxlifetime) { 
    return true; 
} 

session_set_save_handler ("open", "close", "read", "write", "destroy", "gc"); 
session_start();
--EXPECTF--
8
