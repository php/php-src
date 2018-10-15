--TEST--
Bug #70741 (Session WDDX Packet Deserialization Type Confusion Vulnerability)
--SKIPIF--
<?php
if (!extension_loaded("wddx")) print "skip";
if (!extension_loaded("session")) print "skip session extension not available";
?>
--FILE--
<?php
ini_set('session.serialize_handler', 'wddx');
session_start();

$hashtable = str_repeat('A', 66);
$wddx = "<?xml version='1.0'?>
<wddxPacket version='1.0'>
<header/>
	<data>
		<string>$hashtable</string>
	</data>
</wddxPacket>";
session_decode($wddx);
?>
DONE
--EXPECTF--
Warning: session_decode(): Failed to decode session object. Session has been destroyed in %s on line %d
DONE
