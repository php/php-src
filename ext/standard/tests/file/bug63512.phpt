--TEST--
Fixed bug #63512 (parse_ini_file() with INI_SCANNER_RAW removes quotes from value).
--FILE--
<?php

$array = parse_ini_string('
		int = 123
		constant = INSTALL_ROOT
		quotedString = "string"
		a = INSTALL_ROOT "waa"
		b = "INSTALL_ROOT"
		c = "waa" INSTALL_ROOT
		d = INSTALL_ROOT "INSTALL_ROOT"', false, INI_SCANNER_RAW);

var_dump($array);
--EXPECTF--
array(7) {
  ["int"]=>
  string(3) "123"
  ["constant"]=>
  string(12) "INSTALL_ROOT"
  ["quotedString"]=>
  string(6) "string"
  ["a"]=>
  string(18) "INSTALL_ROOT "waa""
  ["b"]=>
  string(12) "INSTALL_ROOT"
  ["c"]=>
  string(18) ""waa" INSTALL_ROOT"
  ["d"]=>
  string(27) "INSTALL_ROOT "INSTALL_ROOT""
}

