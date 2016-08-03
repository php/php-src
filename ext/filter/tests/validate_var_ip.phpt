--TEST--
validate_var() and FILTER_VALIDATE_IP
--SKIPIF--
<?php if (!extension_loaded("filter")) die("skip"); ?>
--FILE--
<?php
try {
	var_dump(validate_var("192.168.0.1", FILTER_VALIDATE_IP));
} catch (Exception $e) {
	var_dump($e->getMessage());
}
try {
	var_dump(validate_var("192.168.0.1.1", FILTER_VALIDATE_IP));
} catch (Exception $e) {
	var_dump($e->getMessage());
}
try {
	var_dump(validate_var("::1", FILTER_VALIDATE_IP));
} catch (Exception $e) {
	var_dump($e->getMessage());
}
try {
	var_dump(validate_var("fe00::0", FILTER_VALIDATE_IP));
} catch (Exception $e) {
	var_dump($e->getMessage());
}
try {
	var_dump(validate_var("::123456", FILTER_VALIDATE_IP));
} catch (Exception $e) {
	var_dump($e->getMessage());
}
try {
	var_dump(validate_var("::1::b", FILTER_VALIDATE_IP));
} catch (Exception $e) {
	var_dump($e->getMessage());
}
try {
	var_dump(validate_var("127.0.0.1", FILTER_VALIDATE_IP));
} catch (Exception $e) {
	var_dump($e->getMessage());
}
try {
	var_dump(validate_var("192.168.0.1", FILTER_VALIDATE_IP, FILTER_FLAG_NO_PRIV_RANGE));
} catch (Exception $e) {
	var_dump($e->getMessage());
}
try {
	var_dump(validate_var("192.0.34.166", FILTER_VALIDATE_IP, FILTER_FLAG_NO_PRIV_RANGE));
} catch (Exception $e) {
	var_dump($e->getMessage());
}
try {
	var_dump(validate_var("127.0.0.1", FILTER_VALIDATE_IP, FILTER_FLAG_NO_RES_RANGE));
} catch (Exception $e) {
	var_dump($e->getMessage());
}
try {
	var_dump(validate_var("192.0.0.1", FILTER_VALIDATE_IP, FILTER_FLAG_NO_RES_RANGE));
} catch (Exception $e) {
	var_dump($e->getMessage());
}
try {
	var_dump(validate_var("100.64.0.0", FILTER_VALIDATE_IP, FILTER_FLAG_NO_RES_RANGE));
} catch (Exception $e) {
	var_dump($e->getMessage());
}
try {
	var_dump(validate_var("100.127.255.255", FILTER_VALIDATE_IP, FILTER_FLAG_NO_RES_RANGE));
} catch (Exception $e) {
	var_dump($e->getMessage());
}
try {
	var_dump(validate_var("192.0.34.166", FILTER_VALIDATE_IP));
} catch (Exception $e) {
	var_dump($e->getMessage());
}
try {
	var_dump(validate_var("256.1237.123.1", FILTER_VALIDATE_IP));
} catch (Exception $e) {
	var_dump($e->getMessage());
}
try {
	var_dump(validate_var("255.255.255.255", FILTER_VALIDATE_IP));
} catch (Exception $e) {
	var_dump($e->getMessage());
}
try {
	var_dump(validate_var("255.255.255.255", FILTER_VALIDATE_IP, FILTER_FLAG_NO_RES_RANGE));
} catch (Exception $e) {
	var_dump($e->getMessage());
}
try {
	var_dump(validate_var("", FILTER_VALIDATE_IP));
} catch (Exception $e) {
	var_dump($e->getMessage());
}
try {
	var_dump(validate_var(-1, FILTER_VALIDATE_IP));
} catch (Exception $e) {
	var_dump($e->getMessage());
}
try {
	var_dump(validate_var("::1", FILTER_VALIDATE_IP, FILTER_FLAG_IPV4));
} catch (Exception $e) {
	var_dump($e->getMessage());
}
try {
	var_dump(validate_var("127.0.0.1", FILTER_VALIDATE_IP, FILTER_FLAG_IPV6));
} catch (Exception $e) {
	var_dump($e->getMessage());
}
try {
	var_dump(validate_var("::1", FILTER_VALIDATE_IP, FILTER_FLAG_IPV6));
} catch (Exception $e) {
	var_dump($e->getMessage());
}
try {
	var_dump(validate_var("::1", FILTER_VALIDATE_IP, FILTER_FLAG_IPV6 | FILTER_FLAG_NO_RES_RANGE));
} catch (Exception $e) {
	var_dump($e->getMessage());
}
try {
	var_dump(validate_var("127.0.0.1", FILTER_VALIDATE_IP, FILTER_FLAG_IPV4));
} catch (Exception $e) {
	var_dump($e->getMessage());
}
try {
	var_dump(validate_var("....", FILTER_VALIDATE_IP));
} catch (Exception $e) {
	var_dump($e->getMessage());
}
try {
	var_dump(validate_var("...", FILTER_VALIDATE_IP));
} catch (Exception $e) {
	var_dump($e->getMessage());
}
try {
	var_dump(validate_var("..", FILTER_VALIDATE_IP));
} catch (Exception $e) {
	var_dump($e->getMessage());
}
try {
	var_dump(validate_var(".", FILTER_VALIDATE_IP));
} catch (Exception $e) {
	var_dump($e->getMessage());
}
try {
	var_dump(validate_var("1.1.1.1", FILTER_VALIDATE_IP));
} catch (Exception $e) {
	var_dump($e->getMessage());
}

echo "Done\n";
?>
--EXPECT--
string(11) "192.168.0.1"
string(43) "IP address validation: Invalid IPv4 address"
string(3) "::1"
string(7) "fe00::0"
string(44) "IP address validataion: Invalid IPv6 address"
string(44) "IP address validataion: Invalid IPv6 address"
string(9) "127.0.0.1"
string(52) "IP address validation: IPv4 address is local address"
string(12) "192.0.34.166"
string(54) "IP address validataion: IPv4 address is reverved range"
string(54) "IP address validataion: IPv4 address is reverved range"
string(54) "IP address validataion: IPv4 address is reverved range"
string(54) "IP address validataion: IPv4 address is reverved range"
string(12) "192.0.34.166"
string(43) "IP address validation: Invalid IPv4 address"
string(15) "255.255.255.255"
string(54) "IP address validataion: IPv4 address is reverved range"
string(47) "IP address validatation: invalid address string"
string(47) "IP address validatation: invalid address string"
string(52) "IP address validation: IPv4 mode, but format is IPv6"
string(52) "IP address validation: IPv6 mode, but format is IPv4"
string(3) "::1"
string(54) "IP address validataion: IPv6 address is reserved range"
string(9) "127.0.0.1"
string(43) "IP address validation: Invalid IPv4 address"
string(43) "IP address validation: Invalid IPv4 address"
string(43) "IP address validation: Invalid IPv4 address"
string(43) "IP address validation: Invalid IPv4 address"
string(7) "1.1.1.1"
Done
