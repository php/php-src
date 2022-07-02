--TEST--
Test session_set_save_handler() : using objects in close
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

echo "*** Testing session_set_save_handler() : using objects in close ***\n";

class MySession7_Foo {
    public $state = 'ok';
    function __destruct() {
        $this->state = 'destroyed';
    }
}

class MySession7 extends SessionHandler {
    public $foo;
    public function close(): bool {
        var_dump($this->foo);
        @var_dump($GLOBALS['bar']);
        return parent::close();
    }
}

$bar = new MySession7_Foo;
$handler = new MySession7;
$handler->foo = new MySession7_Foo;
session_set_save_handler($handler);
session_start();

ob_end_flush();
?>
--EXPECTF--
*** Testing session_set_save_handler() : using objects in close ***
object(MySession7_Foo)#%d (%d) {
  ["state"]=>
  string(2) "ok"
}
object(MySession7_Foo)#%d (%d) {
  ["state"]=>
  string(2) "ok"
}
