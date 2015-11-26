--TEST--
wddx session serializer handler (deserialize)
--SKIPIF--
<?php
	if (!extension_loaded("wddx")) die("skip Wddx module not loaded");
	if (!extension_loaded('session')) die('skip Session module not enabled');

	// following test code stolen from ext/session/skipif.inc
	$save_path = ini_get("session.save_path");
	if ($save_path) {
		if (!file_exists($save_path)) {
			die("skip Session save_path doesn't exist");
		}

		if ($save_path && !@is_writable($save_path)) {
			if (($p = strpos($save_path, ';')) !== false) {
				$save_path = substr($save_path, ++$p);
			}
			if (!@is_writable($save_path)) {
				die("skip\n");
			}
		}
	}
?>
--INI--
precision=14
session.serialize_handler=wddx
session.use_cookies=0
session.cache_limiter=
session.save_handler=files
--FILE--
<?php
	class foo {
		public $bar = "ok";

		function method() { $this->yes = "done"; }
	}

	session_start();

	session_decode("<wddxPacket version='1.0'><header/><data><struct><var name='data'><struct><var name='test1'><boolean value='true'/></var><var name='test2'><string>some string</string></var><var name='test3'><number>654321</number></var><var name='test4'><array length='3'><string>some string</string><boolean value='true'/><null/></array></var></struct></var><var name='class'><struct><var name='php_class_name'><string>foo</string></var><var name='bar'><string>ok</string></var><var name='yes'><string>done</string></var></struct></var></struct></data></wddxPacket>");

	var_dump($_SESSION);

	session_destroy();
?>
--EXPECT--
array(2) {
  ["data"]=>
  array(4) {
    ["test1"]=>
    bool(true)
    ["test2"]=>
    string(11) "some string"
    ["test3"]=>
    int(654321)
    ["test4"]=>
    array(3) {
      [0]=>
      string(11) "some string"
      [1]=>
      bool(true)
      [2]=>
      NULL
    }
  }
  ["class"]=>
  object(foo)#1 (2) {
    ["bar"]=>
    string(2) "ok"
    ["yes"]=>
    string(4) "done"
  }
}
