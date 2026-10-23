--TEST--
Bug #69551 - parse_ini_file() and parse_ini_string() segmentation fault
--FILE--
<?php
$ini = <<<INI
[Network.eth0]
SubnetMask = "
"
INI;
var_dump(parse_ini_string($ini, false, INI_SCANNER_RAW));

echo "Unclosed quote:\n";
$ini = <<<INI
[Network.eth0]
SubnetMask = "
Gateway = 1
INI;
var_dump(parse_ini_string($ini, false, INI_SCANNER_RAW));
?>
--EXPECT--
array(1) {
  ["SubnetMask"]=>
  string(1) "
"
}
Unclosed quote:
array(2) {
  ["SubnetMask"]=>
  string(1) """
  ["Gateway"]=>
  string(1) "1"
}
