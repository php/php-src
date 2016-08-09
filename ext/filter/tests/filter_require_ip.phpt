--TEST--
filter_require_var() and FILTER_VALIDATE_IP
--SKIPIF--
<?php if (!extension_loaded("filter")) die("skip"); ?>
--FILE--
<?php
try {
	var_dump(filter_require_var("192.168.0.1", FILTER_VALIDATE_IP));
} catch (Exception $e) {
	var_dump($e->getMessage());
}
try {
	var_dump(filter_require_var("192.168.0.1.1", FILTER_VALIDATE_IP));
} catch (Exception $e) {
	var_dump($e->getMessage());
}
try {
	var_dump(filter_require_var("::1", FILTER_VALIDATE_IP));
} catch (Exception $e) {
	var_dump($e->getMessage());
}
try {
	var_dump(filter_require_var("fe00::0", FILTER_VALIDATE_IP));
} catch (Exception $e) {
	var_dump($e->getMessage());
}
try {
	var_dump(filter_require_var("::123456", FILTER_VALIDATE_IP));
} catch (Exception $e) {
	var_dump($e->getMessage());
}
try {
	var_dump(filter_require_var("::1::b", FILTER_VALIDATE_IP));
} catch (Exception $e) {
	var_dump($e->getMessage());
}
try {
	var_dump(filter_require_var("127.0.0.1", FILTER_VALIDATE_IP));
} catch (Exception $e) {
	var_dump($e->getMessage());
}
try {
	var_dump(filter_require_var("192.168.0.1", FILTER_VALIDATE_IP, FILTER_FLAG_NO_PRIV_RANGE));
} catch (Exception $e) {
	var_dump($e->getMessage());
}
try {
	var_dump(filter_require_var("192.0.34.166", FILTER_VALIDATE_IP, FILTER_FLAG_NO_PRIV_RANGE));
} catch (Exception $e) {
	var_dump($e->getMessage());
}
try {
	var_dump(filter_require_var("127.0.0.1", FILTER_VALIDATE_IP, FILTER_FLAG_NO_RES_RANGE));
} catch (Exception $e) {
	var_dump($e->getMessage());
}
try {
	var_dump(filter_require_var("192.0.0.1", FILTER_VALIDATE_IP, FILTER_FLAG_NO_RES_RANGE));
} catch (Exception $e) {
	var_dump($e->getMessage());
}
try {
	var_dump(filter_require_var("100.64.0.0", FILTER_VALIDATE_IP, FILTER_FLAG_NO_RES_RANGE));
} catch (Exception $e) {
	var_dump($e->getMessage());
}
try {
	var_dump(filter_require_var("100.127.255.255", FILTER_VALIDATE_IP, FILTER_FLAG_NO_RES_RANGE));
} catch (Exception $e) {
	var_dump($e->getMessage());
}
try {
	var_dump(filter_require_var("192.0.34.166", FILTER_VALIDATE_IP));
} catch (Exception $e) {
	var_dump($e->getMessage());
}
try {
	var_dump(filter_require_var("256.1237.123.1", FILTER_VALIDATE_IP));
} catch (Exception $e) {
	var_dump($e->getMessage());
}
try {
	var_dump(filter_require_var("255.255.255.255", FILTER_VALIDATE_IP));
} catch (Exception $e) {
	var_dump($e->getMessage());
}
try {
	var_dump(filter_require_var("255.255.255.255", FILTER_VALIDATE_IP, FILTER_FLAG_NO_RES_RANGE));
} catch (Exception $e) {
	var_dump($e->getMessage());
}
try {
	var_dump(filter_require_var("", FILTER_VALIDATE_IP));
} catch (Exception $e) {
	var_dump($e->getMessage());
}
try {
	var_dump(filter_require_var(-1, FILTER_VALIDATE_IP));
} catch (Exception $e) {
	var_dump($e->getMessage());
}
try {
	var_dump(filter_require_var("::1", FILTER_VALIDATE_IP, FILTER_FLAG_IPV4));
} catch (Exception $e) {
	var_dump($e->getMessage());
}
try {
	var_dump(filter_require_var("127.0.0.1", FILTER_VALIDATE_IP, FILTER_FLAG_IPV6));
} catch (Exception $e) {
	var_dump($e->getMessage());
}
try {
	var_dump(filter_require_var("::1", FILTER_VALIDATE_IP, FILTER_FLAG_IPV6));
} catch (Exception $e) {
	var_dump($e->getMessage());
}
try {
	var_dump(filter_require_var("::1", FILTER_VALIDATE_IP, FILTER_FLAG_IPV6 | FILTER_FLAG_NO_RES_RANGE));
} catch (Exception $e) {
	var_dump($e->getMessage());
}
try {
	var_dump(filter_require_var("127.0.0.1", FILTER_VALIDATE_IP, FILTER_FLAG_IPV4));
} catch (Exception $e) {
	var_dump($e->getMessage());
}
try {
	var_dump(filter_require_var("....", FILTER_VALIDATE_IP));
} catch (Exception $e) {
	var_dump($e->getMessage());
}
try {
	var_dump(filter_require_var("...", FILTER_VALIDATE_IP));
} catch (Exception $e) {
	var_dump($e->getMessage());
}
try {
	var_dump(filter_require_var("..", FILTER_VALIDATE_IP));
} catch (Exception $e) {
	var_dump($e->getMessage());
}
try {
	var_dump(filter_require_var(".", FILTER_VALIDATE_IP));
} catch (Exception $e) {
	var_dump($e->getMessage());
}
try {
	var_dump(filter_require_var("1.1.1.1", FILTER_VALIDATE_IP));
} catch (Exception $e) {
	var_dump($e->getMessage());
}

echo "Done\n";
?>
--EXPECT--
string(11) "192.168.0.1"
string(112) "IP address validation: Invalid IPv4 address (invalid_key: N/A, filter_name: validate_ip, filter_flags: 33554432)"
string(3) "::1"
string(7) "fe00::0"
string(113) "IP address validataion: Invalid IPv6 address (invalid_key: N/A, filter_name: validate_ip, filter_flags: 33554432)"
string(113) "IP address validataion: Invalid IPv6 address (invalid_key: N/A, filter_name: validate_ip, filter_flags: 33554432)"
string(9) "127.0.0.1"
string(121) "IP address validation: IPv4 address is local address (invalid_key: N/A, filter_name: validate_ip, filter_flags: 41943040)"
string(12) "192.0.34.166"
string(123) "IP address validataion: IPv4 address is reverved range (invalid_key: N/A, filter_name: validate_ip, filter_flags: 37748736)"
string(123) "IP address validataion: IPv4 address is reverved range (invalid_key: N/A, filter_name: validate_ip, filter_flags: 37748736)"
string(123) "IP address validataion: IPv4 address is reverved range (invalid_key: N/A, filter_name: validate_ip, filter_flags: 37748736)"
string(123) "IP address validataion: IPv4 address is reverved range (invalid_key: N/A, filter_name: validate_ip, filter_flags: 37748736)"
string(12) "192.0.34.166"
string(112) "IP address validation: Invalid IPv4 address (invalid_key: N/A, filter_name: validate_ip, filter_flags: 33554432)"
string(15) "255.255.255.255"
string(123) "IP address validataion: IPv4 address is reverved range (invalid_key: N/A, filter_name: validate_ip, filter_flags: 37748736)"
string(116) "IP address validatation: invalid address string (invalid_key: N/A, filter_name: validate_ip, filter_flags: 33554432)"
string(116) "IP address validatation: invalid address string (invalid_key: N/A, filter_name: validate_ip, filter_flags: 33554432)"
string(121) "IP address validation: IPv4 mode, but format is IPv6 (invalid_key: N/A, filter_name: validate_ip, filter_flags: 34603008)"
string(121) "IP address validation: IPv6 mode, but format is IPv4 (invalid_key: N/A, filter_name: validate_ip, filter_flags: 35651584)"
string(3) "::1"
string(123) "IP address validataion: IPv6 address is reserved range (invalid_key: N/A, filter_name: validate_ip, filter_flags: 39845888)"
string(9) "127.0.0.1"
string(112) "IP address validation: Invalid IPv4 address (invalid_key: N/A, filter_name: validate_ip, filter_flags: 33554432)"
string(112) "IP address validation: Invalid IPv4 address (invalid_key: N/A, filter_name: validate_ip, filter_flags: 33554432)"
string(112) "IP address validation: Invalid IPv4 address (invalid_key: N/A, filter_name: validate_ip, filter_flags: 33554432)"
string(112) "IP address validation: Invalid IPv4 address (invalid_key: N/A, filter_name: validate_ip, filter_flags: 33554432)"
string(7) "1.1.1.1"
Done
