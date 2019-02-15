--TEST--
Bug #60749: SNMP module should not strip non-standard SNMP port from hostname
--CREDITS--
Boris Lytochkin
--SKIPIF--
<?php
require_once(dirname(__FILE__).'/skipif.inc');
?>
--FILE--
<?php
require_once(dirname(__FILE__).'/snmp_include.inc');

$hostname = "php.net";
$ip = gethostbyname($hostname);
if (ip2long($ip) === FALSE) {
	echo "Could not resolve $hostname properly!\n";
	exit(1);
}
$port = 1161;
$session = new SNMP(SNMP::VERSION_1, "$hostname:$port", $community, $timeout, $retries);
$info = $session->info;
if (strcmp($info["hostname"], "$ip:$port") !== 0) {
	echo "'" . $info["hostname"] . "' != '$ip:$port'\n";
}
var_dump($session->close());
?>
--EXPECT--
bool(true)
