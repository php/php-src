--TEST--
Test session_set_save_handler() : implicit shutdown
--INI--
session.save_handler=files
session.name=PHPSESSID
--EXTENSIONS--
session
--FILE--
<?php

ob_start();

echo "*** Testing session_set_save_handler() : implicit shutdown ***\n";

class MySession extends SessionHandler {
    public $num;
    public function __construct($num) {
        $this->num = $num;
        echo "(#$this->num) constructor called\n";
    }
    public function __destruct() {
        echo "(#$this->num) destructor called\n";
    }
    public function finish() {
        $id = session_id();
        echo "(#$this->num) finish called $id\n";
        $this->shutdown();
    }
    public function write($id, $data): bool {
        echo "(#$this->num) writing $id = $data\n";
        return parent::write($id, $data);
    }
    public function close(): bool {
        $id = session_id();
        echo "(#$this->num) closing $id\n";
        return parent::close();
    }
}

$handler = new MySession(1);
session_set_save_handler($handler);
session_start();

$_SESSION['foo'] = 'bar';

// implicit close
echo "done\n";
ob_end_flush();
?>
--EXPECTF--
*** Testing session_set_save_handler() : implicit shutdown ***
(#1) constructor called
done
(#1) writing %s = foo|s:3:"bar";
(#1) closing %s
(#1) destructor called
