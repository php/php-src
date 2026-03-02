--TEST--
session_set_save_handler test
--EXTENSIONS--
session
--INI--
session.use_cookies=0
session.use_strict_mode=0
session.cache_limiter=
session.name=PHPSESSID
session.serialize_handler=php
--FILE--
<?php
error_reporting(E_ALL);
ob_start();

class handler implements SessionHandlerInterface {
    public $data = 'baz|O:3:"foo":2:{s:3:"bar";s:2:"ok";s:3:"yes";i:1;}arr|a:1:{i:3;O:3:"foo":2:{s:3:"bar";s:2:"ok";s:3:"yes";i:1;}}';
    function open($save_path, $session_name): bool
    {
        print "OPEN: $session_name\n";
        return true;
    }
    function close(): bool
    {
        return true;
    }
    function read($key): string|false
    {
        print "READ: $key\n";
        return $GLOBALS["hnd"]->data;
    }

    function write($key, $val): bool
    {
        print "WRITE: $key, $val\n";
        $GLOBALS["hnd"]->data = $val;
        return true;
    }

    function destroy($key): bool
    {
        print "DESTROY: $key\n";
        return true;
    }

    function gc($max_lifetime): int { return 1; }
}

$hnd = new handler;

class foo {
    public $bar = "ok";
    public $yes;
    function method() { $this->yes++; }
}

session_set_save_handler(new handler());

session_id("test004");
session_start();
$_SESSION["baz"]->method();
$_SESSION["arr"][3]->method();

var_dump($_SESSION["baz"]);
var_dump($_SESSION["arr"]);

session_write_close();

session_set_save_handler(new handler());
session_start();

var_dump($_SESSION["baz"]);
var_dump($_SESSION["arr"]);

session_destroy();
?>
--EXPECT--
OPEN: PHPSESSID
READ: test004
object(foo)#3 (2) {
  ["bar"]=>
  string(2) "ok"
  ["yes"]=>
  int(2)
}
array(1) {
  [3]=>
  object(foo)#4 (2) {
    ["bar"]=>
    string(2) "ok"
    ["yes"]=>
    int(2)
  }
}
WRITE: test004, baz|O:3:"foo":2:{s:3:"bar";s:2:"ok";s:3:"yes";i:2;}arr|a:1:{i:3;O:3:"foo":2:{s:3:"bar";s:2:"ok";s:3:"yes";i:2;}}
OPEN: PHPSESSID
READ: test004
object(foo)#4 (2) {
  ["bar"]=>
  string(2) "ok"
  ["yes"]=>
  int(2)
}
array(1) {
  [3]=>
  object(foo)#3 (2) {
    ["bar"]=>
    string(2) "ok"
    ["yes"]=>
    int(2)
  }
}
DESTROY: test004
