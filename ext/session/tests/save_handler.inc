<?php

DEFINE("SESSION_FILE_PREFIX" ,"session_test_");
function open($save_path, $session_name) {
    global $session_save_path, $name;
    $session_save_path = $save_path;
    $name = $session_name;
    echo "Open [${session_save_path},${session_name}]\n";
    return true;
}

function close() {
    global $session_save_path, $name;
    echo "Close [${session_save_path},${name}]\n";
    return true;
}

function read($id) {
    global $session_save_path, $name, $session_id;
    $session_id = $id;
    echo "Read [${session_save_path},${id}]\n";
    $session_file = "$session_save_path/".SESSION_FILE_PREFIX.$id;
    // read MUST create file. Otherwise, strict mode will not work
    touch($session_file);
    return (string) @file_get_contents($session_file);
}

function write($id, $session_data) {
    global $session_save_path, $name, $session_id;
    $session_id = $id;
    echo "Write [${session_save_path},${id},${session_data}]\n";
    $session_file = "$session_save_path/".SESSION_FILE_PREFIX.$id;
    if ($fp = fopen($session_file, "w")) {
        $return = fwrite($fp, $session_data);
        fclose($fp);
        return $return === FALSE ? FALSE : TRUE;
    }
    return false;
}

function destroy($id) {
    global $session_save_path, $name;
    echo "Destroy [${session_save_path},${id}]\n";
    $session_file = "$session_save_path/".SESSION_FILE_PREFIX.$id;
    unlink($session_file);
    return true;
}

function gc($maxlifetime) {
    global $session_save_path, $name;
    $directory = opendir($session_save_path."/");
    $length = strlen(SESSION_FILE_PREFIX);
    while (($file = readdir($directory)) !== FALSE) {
        $qualified = ($session_save_path."/".$file); 
        if (is_file($qualified) === TRUE) {
            if (substr($file, 0, $length) === SESSION_FILE_PREFIX) {
                unlink($qualified);
            }
        }
    }
    closedir($directory);
    return true;
}

function create_sid() {
    $id = ('PHPT-'.time());
    echo "CreateID [${id}]\n";
    return $id;
}

function validate_sid($id) {
    global $session_save_path, $name;
    echo "ValidateID [${session_save_path},${id}]\n";
    $session_file = "$session_save_path/".SESSION_FILE_PREFIX.$id;
    $ret = file_exists($session_file);
    return $ret;
}

function update($id, $session_data) {
    global $session_save_path, $name;
    echo "Update [${session_save_path},${id}]\n";
    $session_file = "$session_save_path/".SESSION_FILE_PREFIX.$id;
    $ret = touch($session_file);
    return $ret;
}


function feature() {
    /* NOT IMPLEMENTED YET */
    /* TYPES: gc, create_sid, use_strict_mode, minizie_lock, lazy_write
    /* VALUES: 0=unknown, 1=supported, 2=partially supported, 3=unsupported */
    return array('gc'=>0,
                 'create_sid'=>1,
                 'use_strict_mode'=>2,
                 'minimize_lock'=>3,
                 'lazy_write'=>4,
                 'invalid'=>5,
                 'another invalid'=>6
                 );
}

?>

