--TEST--
Test session_set_save_handler(): incomplete implementation
--INI--
session.save_handler=files
session.name=PHPSESSID
session.gc_probability=0
--EXTENSIONS--
session
--FILE--
<?php

ob_start();

echo "*** Testing session_set_save_handler() : incomplete implementation ***\n";

class MySession6 extends SessionHandler {
    public function open($path, $name): bool {
        // don't call parent
        return true;
    }

    public function read($id): string|false {
        // should error because parent::open hasn't been called
        return parent::read($id);
    }
}

$handler = new MySession6;
session_set_save_handler($handler);
var_dump(session_start());

var_dump(session_id(), ini_get('session.save_handler'), $_SESSION);

session_write_close();
session_unset();
?>
--EXPECTF--
*** Testing session_set_save_handler() : incomplete implementation ***

Warning: SessionHandler::create_sid(): Parent session handler is not open, defaulting to session_create_id() in %s on line %d

Warning: SessionHandler::read(): Parent session handler is not open in %s on line %d

Warning: SessionHandler::close(): Parent session handler is not open in %s on line %d

Warning: session_start(): Failed to read session data: user (%s) in %s on line %d
bool(false)
string(0) ""
string(4) "user"
array(0) {
}
