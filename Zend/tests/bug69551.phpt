--TEST--
Bug #69551 - parse_ini_file() and parse_ini_string() segmentation fault
--FILE--
<?php
$ini = '[Network.eth0]
SubnetMask = "' . "\x0A\x1B" . '"';
parse_ini_string($ini, false, INI_SCANNER_RAW);
echo 'Ready'; // do we reach this line?
?>
--EXPECT--
Ready
