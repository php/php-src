--TEST--
Test ip2long() function : usage variation 2, 32 bit
--SKIPIF--
<?php
if (PHP_INT_SIZE != 4) { die('skip 32 bit only'); }
if (strtolower(substr(PHP_OS, 0, 3)) == 'aix') { die('skip not for AIX'); }
?>
--FILE--
<?php
$ips = array(
    "1.1.011.011",
    "127.0.0.1",
    "1.1.071.071",
    "0.0.0.0",
    "1.1.081.081",
    "192.168.0.0",
    "256.0.0.1",
    "192.168.0xa.5",
);

foreach($ips as $ip) {
    var_dump(ip2long($ip));
}

?>
--EXPECT--
bool(false)
int(2130706433)
bool(false)
int(0)
bool(false)
int(-1062731776)
bool(false)
bool(false)
