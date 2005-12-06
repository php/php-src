--TEST--
filter_data() and FL_IP
--GET--

--FILE--
<?php

var_dump(filter_data("192.168.0.1", FL_IP));
var_dump(filter_data("192.168.0.1.1", FL_IP));
var_dump(filter_data("::1", FL_IP));
var_dump(filter_data("fe00::0", FL_IP));
var_dump(filter_data("::123456", FL_IP));
var_dump(filter_data("::1::b", FL_IP));
var_dump(filter_data("127.0.0.1", FL_IP));
var_dump(filter_data("192.168.0.1", FL_IP, FILTER_FLAG_NO_PRIV_RANGE));
var_dump(filter_data("192.0.34.166", FL_IP, FILTER_FLAG_NO_PRIV_RANGE));
var_dump(filter_data("127.0.0.1", FL_IP, FILTER_FLAG_NO_RES_RANGE));
var_dump(filter_data("192.0.0.1", FL_IP, FILTER_FLAG_NO_RES_RANGE));
var_dump(filter_data("192.0.34.166", FL_IP));
var_dump(filter_data("256.1237.123.1", FL_IP));
var_dump(filter_data("255.255.255.255", FL_IP));
var_dump(filter_data("255.255.255.255", FL_IP, FILTER_FLAG_NO_RES_RANGE));
var_dump(filter_data("", FL_IP));
var_dump(filter_data(-1, FL_IP));


var_dump(filter_data("::1", FL_IP, FILTER_FLAG_IPV4));
var_dump(filter_data("127.0.0.1", FL_IP, FILTER_FLAG_IPV6));

var_dump(filter_data("::1", FL_IP, FILTER_FLAG_IPV6));
var_dump(filter_data("127.0.0.1", FL_IP, FILTER_FLAG_IPV4));

echo "Done\n";
?>
--EXPECT--	
string(11) "192.168.0.1"
NULL
string(3) "::1"
string(7) "fe00::0"
NULL
NULL
string(9) "127.0.0.1"
NULL
string(12) "192.0.34.166"
string(9) "127.0.0.1"
string(9) "192.0.0.1"
string(12) "192.0.34.166"
NULL
string(15) "255.255.255.255"
NULL
NULL
NULL
NULL
NULL
string(3) "::1"
string(9) "127.0.0.1"
Done
