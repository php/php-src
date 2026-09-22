--TEST--
snmp: resolved address list is freed when peername resolution drains empty
--EXTENSIONS--
snmp
--FILE--
<?php
// Bracketed (IPv6) syntax over an IPv4-only literal forces IPv6 resolution,
// which drains to no usable address, hitting the "Unknown failure while
// resolving" path. That path must free the address list from
// php_network_getaddresses() (it previously leaked it). No SNMP agent needed:
// the function fails at peername resolution before any query is sent.
var_dump(snmpget("[127.0.0.1]", "public", ".1.3.6.1.2.1.1.1.0"));
?>
--EXPECTF--
Warning: snmpget(): Unknown failure while resolving '[127.0.0.1]' in %s on line %d
bool(false)
