--TEST--
FILTER_VALIDATE_IP: the NAT64 local-use prefix 64:ff9b:1::/48 (RFC 8215) is private
--EXTENSIONS--
filter
--FILE--
<?php
$addresses = [
    // 64:ff9b::/96 (RFC 6052): globally reachable if the embedded IPv4 is
    '64:ff9b::808:808',
    '64:ff9b::1',
    '64:ff9b::a9fe:a9fe',
    '64:ff9b::ffff:ffff',
    // 64:ff9b:1::/48 (RFC 8215): local use, private
    '64:ff9b:1::',
    '64:ff9b:1::1',
    '64:ff9b:1::a9fe:a9fe',
    '64:ff9b:1:ffff:ffff:ffff:ffff:ffff',
    // Rest of 64:ff9b::/32: not a special-purpose block
    '64:ff9b:2::1',
    '64:ff9b:0:1::1',
    '64:ff9b::1:0:0',
    '64:ff9b:0:0:1::',
    '64:ff9b:2::',
];

foreach ($addresses as $address) {
    echo $address, "\n";
    echo '  GLOBAL_RANGE:  ';
    var_dump(filter_var($address, FILTER_VALIDATE_IP, FILTER_FLAG_IPV6 | FILTER_FLAG_GLOBAL_RANGE));
    echo '  NO_PRIV|NO_RES: ';
    var_dump(filter_var($address, FILTER_VALIDATE_IP, FILTER_FLAG_IPV6 | FILTER_FLAG_NO_PRIV_RANGE | FILTER_FLAG_NO_RES_RANGE));
}
?>
--EXPECT--
64:ff9b::808:808
  GLOBAL_RANGE:  string(16) "64:ff9b::808:808"
  NO_PRIV|NO_RES: string(16) "64:ff9b::808:808"
64:ff9b::1
  GLOBAL_RANGE:  bool(false)
  NO_PRIV|NO_RES: bool(false)
64:ff9b::a9fe:a9fe
  GLOBAL_RANGE:  bool(false)
  NO_PRIV|NO_RES: bool(false)
64:ff9b::ffff:ffff
  GLOBAL_RANGE:  bool(false)
  NO_PRIV|NO_RES: bool(false)
64:ff9b:1::
  GLOBAL_RANGE:  bool(false)
  NO_PRIV|NO_RES: bool(false)
64:ff9b:1::1
  GLOBAL_RANGE:  bool(false)
  NO_PRIV|NO_RES: bool(false)
64:ff9b:1::a9fe:a9fe
  GLOBAL_RANGE:  bool(false)
  NO_PRIV|NO_RES: bool(false)
64:ff9b:1:ffff:ffff:ffff:ffff:ffff
  GLOBAL_RANGE:  bool(false)
  NO_PRIV|NO_RES: bool(false)
64:ff9b:2::1
  GLOBAL_RANGE:  string(12) "64:ff9b:2::1"
  NO_PRIV|NO_RES: string(12) "64:ff9b:2::1"
64:ff9b:0:1::1
  GLOBAL_RANGE:  string(14) "64:ff9b:0:1::1"
  NO_PRIV|NO_RES: string(14) "64:ff9b:0:1::1"
64:ff9b::1:0:0
  GLOBAL_RANGE:  string(14) "64:ff9b::1:0:0"
  NO_PRIV|NO_RES: string(14) "64:ff9b::1:0:0"
64:ff9b:0:0:1::
  GLOBAL_RANGE:  string(15) "64:ff9b:0:0:1::"
  NO_PRIV|NO_RES: string(15) "64:ff9b:0:0:1::"
64:ff9b:2::
  GLOBAL_RANGE:  string(11) "64:ff9b:2::"
  NO_PRIV|NO_RES: string(11) "64:ff9b:2::"
