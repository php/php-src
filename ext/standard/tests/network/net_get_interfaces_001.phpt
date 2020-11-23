--TEST--
net_get_interfaces IPv4 Loopback
--SKIPIF--
<?php
function_exists('net_get_interfaces') || print 'skip';
--FILE--
<?php

// Test that we have exactly one unicast address with the address 127.0.0.1
// On linux this will often be named "lo", but even that isn't guaranteed.

$ifaces = net_get_interfaces();

$found = false;
foreach ($ifaces as $iface) {
  foreach ($iface['unicast'] as $unicast) {
    if (($unicast['address'] ?? null) === '127.0.0.1') {
      $found = true;
      break 2;
    }
  }
}

var_dump($found);
if (!$found) {
  // Extra diagnostics!
  var_dump($ifaces);
}
?>
--EXPECT--
bool(true)
