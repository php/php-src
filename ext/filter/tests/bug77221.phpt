--TEST--
Bug #77221 (Request IP address filter flag to exclude non-global IP addresses)
--SKIPIF--
<?php
if (!extension_loaded('filter')) die("skip filter extension not available");
?>
--FILE--
<?php

$non_global_ranges = [];
$non_global_ranges['0.0.0.0/8'] = ['0.0.0.0', '0.255.255.255'];
$non_global_ranges['10.0.0.0/8'] = ['10.0.0.0', '10.255.255.255'];
$non_global_ranges['100.64.0.0/10'] = ['100.64.0.0', '100.127.255.255'];
$non_global_ranges['127.0.0.0/8'] = ['127.0.0.0', '127.255.255.255'];
$non_global_ranges['169.254.0.0/16'] = ['169.254.0.0', '169.254.255.255'];
$non_global_ranges['172.16.0.0/12'] = ['172.16.0.0', '172.31.255.255'];
$non_global_ranges['192.0.0.0/24'] = ['192.0.0.0', '192.0.0.255'];
$non_global_ranges['192.0.2.0/24'] = ['192.0.2.0', '192.0.2.255'];
$non_global_ranges['192.168.0.0/16'] = ['192.168.0.0', '192.168.255.255'];
$non_global_ranges['198.18.0.0/15'] = ['198.18.0.0', '198.19.255.255'];
$non_global_ranges['198.51.100.0/24'] = ['198.51.100.0', '198.51.100.255'];
$non_global_ranges['203.0.113.0/24'] = ['203.0.113.0', '203.0.113.255'];
$non_global_ranges['240.0.0.0/4'] = ['240.0.0.0', '255.255.255.255'];

$non_global_ranges['::/128'] = ['::0', '::'];
$non_global_ranges['::1/128'] = ['0000:0000:0000:0000:0000:0000:0000:1', '0:0:0:0:0:0:0:1'];
$non_global_ranges['::ffff:0:0/96'] = ['::ffff:0:0', '::ffff:ffff:ffff'];
$non_global_ranges['100::/64'] = ['0100::', '100::ffff:ffff:ffff:ffff'];
$non_global_ranges['2001::/23'] = ['2001::', '2001:01ff:ffff:ffff:ffff:ffff:ffff:ffff'];
$non_global_ranges['2001:2::/48'] = ['2001:2::', '2001:2:0:ffff:ffff:ffff:ffff:ffff'];
$non_global_ranges['2001:db8::/32'] = ['2001:db8::', '2001:db8:ffff:ffff:ffff:ffff:ffff:ffff'];
$non_global_ranges['2001:10::/28'] = ['2001:10::', '2001:1f:ffff:ffff:ffff:ffff:ffff:ffff'];
$non_global_ranges['fc00::/7'] = ['fc00::', 'fdff:ffff:ffff:ffff:ffff:ffff:ffff:ffff'];
$non_global_ranges['fe80::/10'] = ['fe80::', 'febf:ffff:ffff:ffff:ffff:ffff:ffff:ffff'];


foreach ($non_global_ranges as $key => $range) {
    list($min, $max) = $range;
    var_dump($key);
    var_dump(filter_var($min, FILTER_VALIDATE_IP, FILTER_FLAG_GLOBAL_RANGE));
    var_dump(filter_var($max, FILTER_VALIDATE_IP, FILTER_FLAG_GLOBAL_RANGE));
}

var_dump(filter_var('185.85.0.29', FILTER_VALIDATE_IP, FILTER_FLAG_GLOBAL_RANGE));
var_dump(filter_var('64:ff9b::', FILTER_VALIDATE_IP, FILTER_FLAG_GLOBAL_RANGE));
var_dump(filter_var('64:ff9b::ffff', FILTER_VALIDATE_IP, FILTER_FLAG_GLOBAL_RANGE));

?>
--EXPECT--
string(9) "0.0.0.0/8"
bool(false)
bool(false)
string(10) "10.0.0.0/8"
bool(false)
bool(false)
string(13) "100.64.0.0/10"
bool(false)
bool(false)
string(11) "127.0.0.0/8"
bool(false)
bool(false)
string(14) "169.254.0.0/16"
bool(false)
bool(false)
string(13) "172.16.0.0/12"
bool(false)
bool(false)
string(12) "192.0.0.0/24"
bool(false)
bool(false)
string(12) "192.0.2.0/24"
bool(false)
bool(false)
string(14) "192.168.0.0/16"
bool(false)
bool(false)
string(13) "198.18.0.0/15"
bool(false)
bool(false)
string(15) "198.51.100.0/24"
bool(false)
bool(false)
string(14) "203.0.113.0/24"
bool(false)
bool(false)
string(11) "240.0.0.0/4"
bool(false)
bool(false)
string(6) "::/128"
bool(false)
bool(false)
string(7) "::1/128"
bool(false)
bool(false)
string(13) "::ffff:0:0/96"
bool(false)
bool(false)
string(8) "100::/64"
bool(false)
bool(false)
string(9) "2001::/23"
bool(false)
bool(false)
string(11) "2001:2::/48"
bool(false)
bool(false)
string(13) "2001:db8::/32"
bool(false)
bool(false)
string(12) "2001:10::/28"
bool(false)
bool(false)
string(8) "fc00::/7"
bool(false)
bool(false)
string(9) "fe80::/10"
bool(false)
bool(false)
string(11) "185.85.0.29"
string(9) "64:ff9b::"
string(13) "64:ff9b::ffff"
