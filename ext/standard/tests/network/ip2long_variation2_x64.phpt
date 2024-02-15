--TEST--
Test ip2long() function : usage variation 2, 64 bit
--SKIPIF--
<?php
/* from man inet_pton :
     All numbers supplied as ``parts'' in a `.' notation may be decimal, octal, or hexadecimal, as specified
     in the C language (i.e., a leading 0x or 0X implies hexadecimal; otherwise, a leading 0 implies octal;
     otherwise, the number is interpreted as decimal).
*/
if(PHP_INT_SIZE != 8) {die('skip 64 bit only');}
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
int(3232235520)
bool(false)
bool(false)
