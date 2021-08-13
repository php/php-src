--TEST--
Test session_set_save_handler() : session_gc() returns the number of deleted records.
--INI--
session.name=PHPSESSID
session.save_handler=files
session.gc_probability=0
--EXTENSIONS--
session
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php

ob_start();

echo "*** Test session_set_save_handler() : session_gc() returns the number of deleted records. ***\n";

class MySession implements SessionHandlerInterface {
    public function open($path, $name): bool {
        echo 'Open', "\n";
        return true;
    }
    public function read($key): string|false {
        echo 'Read ', session_id(), "\n";
        return '';
    }
    public function write($key, $data): bool {
        echo 'Write ', session_id(), "\n";
        return true;
    }
    public function close(): bool {
        echo 'Close ', session_id(), "\n";
        return true;
    }
    public function destroy($key): bool {
        echo 'Destroy ', session_id(), "\n";
        return true;
    }
    public function gc($ts): int|false {
        echo 'Garbage collect', "\n";
        return 1;
    }
}

$handler = new MySession;
session_set_save_handler($handler);
session_start();
var_dump(session_gc());
session_write_close();

?>
--EXPECTF--
*** Test session_set_save_handler() : session_gc() returns the number of deleted records. ***
Open
Read %s
Garbage collect
int(1)
Write %s
Close %s
