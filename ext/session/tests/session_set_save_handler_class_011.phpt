--TEST--
Test session_set_save_handler() : shutdown failure
--INI--
session.save_handler=files
session.name=PHPSESSID
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

echo "*** Testing session_set_save_handler() : shutdown failure ***\n";

class MySession extends SessionHandler {
    public $num;
    public $destroyed = false;
    public function __construct($num) {
        $this->num = $num;
        echo "(#$this->num) constructor called\n";
    }
    public function __destruct() {
        echo "(#$this->num) destructor called\n";
        $this->destroyed = true;
    }
    public function write($id, $data) {
        if ($this->destroyed) {
            echo "(#$this->num) destroyed, cannot write\n";
        } else {
            echo "(#$this->num) writing $id = $data\n";
        }
        return parent::write($id, $data);
    }
    public function close() {
        $id = session_id();
        if ($this->destroyed) {
            echo "(#$this->num) destroyed, cannot write\n";
        } else {
            echo "(#$this->num) closing $id\n";
        }
        return parent::close();
    }
}

$handler = new MySession(1);
session_set_save_handler($handler, false);
session_start();

$_SESSION['foo'] = 'bar';

echo "done\n";
ob_end_flush();
?>
--EXPECT--
*** Testing session_set_save_handler() : shutdown failure ***
(#1) constructor called
done
(#1) destructor called
(#1) destroyed, cannot write
(#1) destroyed, cannot write
