--TEST--
Test session_set_save_handler() : session_gc() returns the number of deleted records.
--INI--
session.name=PHPSESSID
session.save_handler=files
session.gc_probability=0
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php

ob_start();

/*
 * Prototype : bool session_set_save_handler(SessionHandler $handler [, bool $register_shutdown_function = true])
 * Description : Sets user-level session storage functions
 * Source code : ext/session/session.c
 */

echo "*** Test session_set_save_handler() : session_gc() returns the number of deleted records. ***\n";

class MySession implements SessionHandlerInterface {
    public function open($path, $name) {
        echo 'Open', "\n";
        return true;
    }
    public function read($key) {
        echo 'Read ', session_id(), "\n";
        return '';
    }
    public function write($key, $data) {
        echo 'Write ', session_id(), "\n";
        return true;
    }
    public function close() {
        echo 'Close ', session_id(), "\n";
        return true;
    }
    public function destroy($key) {
        echo 'Destroy ', session_id(), "\n";
        return true;
    }
    public function gc($ts) {
        echo 'Garbage collect', "\n";
        return 1;
    }
}

$handler = new MySession;
session_set_save_handler($handler);
session_start();
var_dump(session_gc());
session_write_close();

--EXPECTF--
*** Test session_set_save_handler() : session_gc() returns the number of deleted records. ***
Open
Read %s
Garbage collect
int(1)
Write %s
Close %s
