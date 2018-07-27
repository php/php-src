--TEST--
Bug #74603 (PHP INI Parsing Stack Buffer Overflow Vulnerability)
--FILE--
<?php
var_dump(parse_ini_file(__DIR__ . "/bug74603.ini", true, INI_SCANNER_NORMAL));
?>
--EXPECT--
array(1) {
  [0]=>
  string(1) "0"
}
