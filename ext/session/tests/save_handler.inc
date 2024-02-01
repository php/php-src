<?php

DEFINE("SESSION_FILE_PREFIX" ,"session_test_");

/*
 * == General Return Value Rule ==
 *
 * Returning FALSE indicates FATAL error.
 * Exceptions are: gc(), validate_sid()
 *
 * == Session Data Lock ==
 *
 * Session data lock is mandatory. Lock must be exclusive. i.e. Block read also.
 *
 * == Collision Detection ==
 *
 * Collision detection is mandatory to reject attacker initialized session ID.
 * Coolision detection is absolute requirement for secure session.
 */


/* Open session data database */
function open($save_path, $session_name) {
    // string $save_path - Directory path, connection strings, etc. Default: session.save_path
    // string $session_name - Session ID cookie name. Default: session.name

    global $session_save_path, $name;
    $session_save_path = $save_path;
    $name = $session_name;
    echo "Open [{$session_save_path},{$session_name}]\n";

    // MUST return bool. Return TRUE for success.
    return true;
}

/* Close session data database */
function close() {
    // void parameter
    // NOTE: This function should unlock session data, if write() does not unlock it.

    global $session_save_path, $name;
    echo "Close [{$session_save_path},{$name}]\n";

    // MUST return bool. Return TRUE for success.
    return true;
}

/* Read session data */
function read($id) {
    // string $id - Session ID string
    // NOTE: All production session save handler MUST implement "exclusive" lock.
    //       e.g. Use "serializable transaction isolation level" with RDBMS.
    //       read() would be the best place for locking for most save handlers.

    global $session_save_path, $name, $session_id;
    $session_id = $id;
    echo "Read [{$session_save_path},{$id}]\n";
    $session_file = "$session_save_path/".SESSION_FILE_PREFIX.$id;
    // read MUST create file. Otherwise, strict mode will not work
    touch($session_file);

    // MUST return STRING for successful read().
    // Return FALSE only when there is error. i.e. Do not return FALSE
    // for non-existing session data for the $id.
    return (string) @file_get_contents($session_file);
}

/* Write session data */
function write($id, $session_data) {
    // string $id - Session ID string
    // string $session_data - Session data string serialized by session serializer.
    // NOTE: This function may unlock session data locked by read(). If write() is
    //       is not suitable place your handler to unlock. Unlock data at close().

    global $session_save_path, $name, $session_id;
    $session_id = $id;
    echo "Write [{$session_save_path},{$id},{$session_data}]\n";
    $session_file = "$session_save_path/".SESSION_FILE_PREFIX.$id;
    if ($fp = fopen($session_file, "w")) {
        $return = fwrite($fp, $session_data);
        fclose($fp);
        return $return === FALSE ? FALSE : TRUE;
    }

    // MUST return bool. Return TRUE for success.
    return false;
}

/* Remove specified session */
function destroy($id) {
    // string $id - Session ID string

    global $session_save_path, $name;
    echo "Destroy [{$session_save_path},{$id}]\n";
    $session_file = "$session_save_path/".SESSION_FILE_PREFIX.$id;
    unlink($session_file);

    // MUST return bool. Return TRUE for success.
    // Return FALSE only when there is error. i.e. Do not return FALSE
    // for non-existing session data for the $id.
    return true;
}

/* Perform garbage collection */
function gc($maxlifetime) {
    // long $maxlifetime - GC TTL in seconds. Default: session.gc_maxlifetime

    global $session_save_path, $name;
    $gc_cnt = 0;
    $directory = opendir($session_save_path."/");
    $length = strlen(SESSION_FILE_PREFIX);
    while (($file = readdir($directory)) !== FALSE) {
        $qualified = ($session_save_path."/".$file);
        if (is_file($qualified) === TRUE) {
            if (substr($file, 0, $length) === SESSION_FILE_PREFIX && (filemtime($qualified) + $maxlifetime <= time() )) {
                unlink($qualified);
                $gc_cnt++;
            }
        }
    }
    closedir($directory);

    // SHOULD return long (number of deleted sessions).
    // Returning TRUE works also, but it will not report correct number of deleted sessions.
    // Return negative value for error. FALSE does not work because it's the same as 0.
    return $gc_cnt;
}

/* Create new secure session ID */
function create_sid() {
    // void parameter
    // NOTE: Defining create_sid() is mandatory because validate_sid() is mandatory for
    //       security reasons for production save handler.
    //       PHP 7.1 has session_create_id() for secure session ID generation. Older PHPs
    //       must generate secure session ID by yourself.
    //       e.g. hash('sha2', random_bytes(64)) or use /dev/urandom

    $id = ('PHPT-'.time());
    echo "CreateID [{$id}]\n";

    // MUST return session ID string.
    // Return FALSE for error.
    return $id;
}

/* Check session ID collision */
function validate_sid($id) {
    // string $id - Session ID string

    global $session_save_path, $name;
    echo "ValidateID [{$session_save_path},{$id}]\n";
    $session_file = "$session_save_path/".SESSION_FILE_PREFIX.$id;
    $ret = file_exists($session_file);

    // MUST return bool. Return TRUE for collision.
    // NOTE: This handler is mandatory for session security.
    //       All save handlers MUST implement this handler.
    //       Check session ID collision, return TRUE when it collides.
    //       Otherwise, return FALSE.
    return $ret;
}

/* Update session data access time stamp WITHOUT writing $session_data */
function update($id, $session_data) {
    // string $id - Session ID string
    // string $session_data - Session data serialized by session serializer
    // NOTE: This handler is optional. If your session database cannot
    //       support time stamp updating, you must not define this.

    global $session_save_path, $name;
    echo "Update [{$session_save_path},{$id}]\n";
    $session_file = "$session_save_path/".SESSION_FILE_PREFIX.$id;
    $ret = touch($session_file);

    // MUST return bool. Return TRUE for success.
    return $ret;
}


function feature() {
    /* NOT IMPLEMENTED YET */
    /* TYPES: gc, create_sid, use_strict_mode, minimzie_lock, lazy_write
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
