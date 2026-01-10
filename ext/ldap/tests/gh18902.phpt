--TEST--
GH-17704 (ldap_search fails when $attributes contains a non-packed array with numerical keys)
--EXTENSIONS--
ldap
--FILE--
<?php
$conn = ldap_connect();

try {
	ldap_exop($conn,"\0");
} catch (\ValueError $e) {
	echo $e->getMessage(), PHP_EOL;
}

try {
	ldap_exop_sync($conn,"");
} catch (\ValueError $e) {
	echo $e->getMessage(), PHP_EOL;
}

try {
	ldap_exop_sync($conn,"test\0");
} catch (\ValueError $e) {
	echo $e->getMessage(), PHP_EOL;
}
?>
--EXPECTF--
ldap_exop(): Argument #2 ($request_oid) must not contain any null bytes
ldap_exop_sync(): Argument #2 ($request_oid) must not be empty
ldap_exop_sync(): Argument #2 ($request_oid) must not contain any null bytes
