--TEST--
serializing references test case using globals
--SKIPIF--
<?php include('skipif.inc'); ?>
--INI--
session.use_cookies=0
session.cache_limiter=
register_globals=1
session.serialize_handler=php
session.save_handler=files
--FILE--
<?php

error_reporting(E_ALL);

class TFoo {
	public $c;
	function TFoo($c) {
		$this->c = $c;
	}
	function inc() {
		$this->c++;
	}
}

session_id("abtest");
session_start();
session_register('o1', 'o2' );

$o1 = new TFoo(42);
$o2 =& $o1;

session_write_close();

unset($o1);
unset($o2);

session_start();

var_dump($_SESSION);

$o1->inc();
$o2->inc();

var_dump($_SESSION);

session_destroy();
?>
--EXPECTF--
array(2) {
  ["o1"]=>
  &object(TFoo)#%d (1) {
    ["c"]=>
    int(42)
  }
  ["o2"]=>
  &object(TFoo)#%d (1) {
    ["c"]=>
    int(42)
  }
}
array(2) {
  ["o1"]=>
  &object(TFoo)#%d (1) {
    ["c"]=>
    int(44)
  }
  ["o2"]=>
  &object(TFoo)#%d (1) {
    ["c"]=>
    int(44)
  }
}
