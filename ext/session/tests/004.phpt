--TEST--
session_set_save_handler test
--SKIPIF--
<?php include('skipif.inc'); ?>
--INI--
register_globals=1
--FILE--
<?php
error_reporting(~E_NOTICE);

class handler {
	var $data = 'baz|O:3:"foo":2:{s:3:"bar";s:2:"ok";s:3:"yes";i:1;}arr|a:1:{i:3;O:3:"foo":2:{s:3:"bar";s:2:"ok";s:3:"yes";i:1;}}';
    function open($save_path, $session_name)
    {
        print "OPEN: $session_name\n";
        return true;
    }
    function close()
    {
        return true;
    }
    function read($key)
    {
        print "READ: $key\n";
        return $GLOBALS["hnd"]->data;
    }

    function write($key, $val)
    {
        print "WRITE: $key, $val\n";
		$GLOBALS["hnd"]->data = $val;
        return true;
    }

    function destroy($key)
    {
        print "DESTROY: $key\n";
        return true;
    }

    function gc() { return true; }
}

$hnd = new handler;

class foo {
    var $bar = "ok";
    function method() { $this->yes++; }
}

ob_start();

session_set_save_handler(array($hnd, "open"), array($hnd, "close"), array($hnd, "read"), array($hnd, "write"), array($hnd, "destroy"), array($hnd, "gc"));

session_id("abtest");
session_start();
$baz->method();
$arr[3]->method();

var_dump($baz);
var_dump($arr);

session_write_close();

session_set_save_handler(array($hnd, "open"), array($hnd, "close"), array($hnd, "read"), array($hnd, "write"), array($hnd, "destroy"), array($hnd, "gc"));
session_start();

var_dump($baz);
var_dump($arr);

session_destroy();
?>
--EXPECT--
OPEN: PHPSESSID
READ: abtest
object(foo)(2) {
  ["bar"]=>
  string(2) "ok"
  ["yes"]=>
  int(2)
}
array(1) {
  [3]=>
  &object(foo)(2) {
    ["bar"]=>
    string(2) "ok"
    ["yes"]=>
    int(2)
  }
}
WRITE: abtest, baz|O:3:"foo":2:{s:3:"bar";s:2:"ok";s:3:"yes";i:2;}arr|a:1:{i:3;O:3:"foo":2:{s:3:"bar";s:2:"ok";s:3:"yes";i:2;}}
OPEN: PHPSESSID
READ: abtest
object(foo)(2) {
  ["bar"]=>
  string(2) "ok"
  ["yes"]=>
  int(2)
}
array(1) {
  [3]=>
  object(foo)(2) {
    ["bar"]=>
    string(2) "ok"
    ["yes"]=>
    int(2)
  }
}
DESTROY: abtest
