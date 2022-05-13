--TEST--
Test session_set_save_handler() : incorrect arguments for existing handler open
--INI--
session.save_handler=files
session.name=PHPSESSID
session.gc_probability=0
--EXTENSIONS--
session
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php

ob_start();

echo "*** Testing session_set_save_handler() : incorrect arguments for existing handler open ***\n";

class MySession extends SessionHandler {
    public $i = 0;
    public function open($path, $name): bool {
        ++$this->i;
        echo 'Open ', session_id(), "\n";
        // This test was written for broken return value handling
        // Mimmick what was actually being tested by returning true here
        return (null === parent::open());
    }
    public function read($key): string|false {
        ++$this->i;
        echo 'Read ', session_id(), "\n";
        return parent::read($key);
    }
}

$oldHandler = ini_get('session.save_handler');
$handler = new MySession;
session_set_save_handler($handler);
try {
    var_dump(session_start());
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

var_dump(session_id(), $oldHandler, ini_get('session.save_handler'), $handler->i, $_SESSION);
?>
--EXPECTF--
*** Testing session_set_save_handler() : incorrect arguments for existing handler open ***
Open 
SessionHandler::open() expects exactly 2 arguments, 0 given

Warning: Undefined global variable $_SESSION in %s on line %d
string(0) ""
string(5) "files"
string(4) "user"
int(1)
NULL
