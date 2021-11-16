--TEST--
Test session_set_save_handler() : incorrect arguments for existing handler close
--INI--
session.save_handler=files
session.name=PHPSESSID
--EXTENSIONS--
session
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php

ob_start();

echo "*** Testing session_set_save_handler() : incorrect arguments for existing handler close ***\n";

class MySession extends SessionHandler {
    public $i = 0;
    public function open($path, $name): bool {
        ++$this->i;
        echo 'Open ', session_id(), "\n";
        return parent::open($path, $name);
    }
    public function read($key): string|false {
        ++$this->i;
        echo 'Read ', session_id(), "\n";
        return parent::read($key);
    }
    public function close(): bool {
        return parent::close(false);
    }
}

$oldHandler = ini_get('session.save_handler');
$handler = new MySession;
session_set_save_handler($handler);
session_start();

var_dump(session_id(), $oldHandler, ini_get('session.save_handler'), $handler->i, $_SESSION);
?>
--EXPECTF--
*** Testing session_set_save_handler() : incorrect arguments for existing handler close ***
Open 
Read %s
string(%d) "%s"
string(5) "files"
string(4) "user"
int(2)
array(0) {
}

Fatal error: Uncaught ArgumentCountError: SessionHandler::close() expects exactly 0 arguments, 1 given in %s:%d
Stack trace:
#0 %s(%d): SessionHandler->close(false)
#1 [internal function]: MySession->close()
#2 [internal function]: session_write_close()
#3 {main}
  thrown in %s on line %d
