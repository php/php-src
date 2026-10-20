--TEST--
Check for libsodium bin2ip/ip2bin helpers
--EXTENSIONS--
sodium
--SKIPIF--
<?php
if (!defined('SODIUM_CRYPTO_IPCRYPT_KEYBYTES')) print "skip libsodium without ipcrypt (requires >= 1.0.21)";
?>
--FILE--
<?php
/* IPv4 roundtrip */
$bin = sodium_ip2bin("192.168.1.1");
var_dump(strlen($bin));
var_dump(bin2hex($bin));
var_dump(sodium_bin2ip($bin));

/* IPv6 roundtrip */
$bin6 = sodium_ip2bin("::1");
var_dump(bin2hex($bin6));
var_dump(sodium_bin2ip($bin6));

/* IPv4-mapped IPv6 preserved as IPv4 */
$bin4 = sodium_ip2bin("10.0.0.1");
var_dump(bin2hex($bin4));
var_dump(sodium_bin2ip($bin4));

/* Full IPv6 address */
$bin_full = sodium_ip2bin("fe80::1");
var_dump(bin2hex($bin_full));
var_dump(sodium_bin2ip($bin_full));

/* ip2bin -> bin2ip roundtrip for various addresses */
$addrs = ["0.0.0.0", "255.255.255.255", "127.0.0.1", "::ffff:192.168.0.1", "2001:db8::1"];
foreach ($addrs as $addr) {
    $result = sodium_bin2ip(sodium_ip2bin($addr));
    echo "$addr => $result\n";
}

/* Error: bin2ip wrong length */
try {
    sodium_bin2ip("short");
} catch (SodiumException $e) {
    echo $e->getMessage() . "\n";
}

/* Error: ip2bin invalid address */
try {
    sodium_ip2bin("not_an_ip");
} catch (SodiumException $e) {
    echo $e->getMessage() . "\n";
}
?>
--EXPECT--
int(16)
string(32) "00000000000000000000ffffc0a80101"
string(11) "192.168.1.1"
string(32) "00000000000000000000000000000001"
string(3) "::1"
string(32) "00000000000000000000ffff0a000001"
string(8) "10.0.0.1"
string(32) "fe800000000000000000000000000001"
string(7) "fe80::1"
0.0.0.0 => 0.0.0.0
255.255.255.255 => 255.255.255.255
127.0.0.1 => 127.0.0.1
::ffff:192.168.0.1 => 192.168.0.1
2001:db8::1 => 2001:db8::1
sodium_bin2ip(): Argument #1 ($bin) must be 16 bytes long
sodium_ip2bin(): Argument #1 ($ip) must be a valid IP address
