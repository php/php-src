--TEST--
Bug #76248 (Malicious LDAP-Server Response causes Crash)
--SKIPIF--
<?php
require_once('skipif.inc');
if (!function_exists('pcntl_fork')) die('skip fork not available');
?>
--FILE--
<?php
$pid = pcntl_fork();
const PORT = 12345;
if ($pid == 0) {
	// child
    $server = stream_socket_server("tcp://127.0.0.1:12345");
	$socket = stream_socket_accept($server, 3);
	fwrite($socket, base64_decode("MAwCAQFhBwoBAAQABAAweQIBAmR0BJljbj1yb290LGRjPWV4YW1wbGUsZGM9Y29tMFcwIwQLb2JqZWN0Q2xhc3MxFAQSb3JnYW5pemF0aW9uYWxSb2xlMAwEAmNuMQYEBHJvb3QwIgQLZGVzY3JpcHRpb24xEwQRRGlyZWN0b3J5IE1hbmFnZXIwDAIBAmUHCgEABAAEADB5AgEDZHQEmWNuPXJvb3QsZGM9ZXhhbXBsZSxkYz1jb20wVzAjBAtvYmplY3RDbGFzczEUBBJvcmdhbml6YXRpb25hbFJvbGUwDAQCY24xBgQEcm9vdDAiBAtkZXNjcmlwdGlvbjETBBFEaXJlY3RvcnkgTWFuYWdlcjAMAgEDZQcKAQAEAAQA"));
	fflush($socket);
} else {
	// parent
	$ds = ldap_connect("127.0.0.1", PORT);
	ldap_set_option($ds, LDAP_OPT_PROTOCOL_VERSION, 3);
	$b = ldap_bind($ds, "cn=root,dc=example,dc=com", "secret");

	$s = ldap_search($ds, "dc=example,dc=com", "(cn=root)");
	$tt = ldap_get_entries($ds, $s);
	var_dump($tt);
}
?>
--EXPECT--
array(2) {
  ["count"]=>
  int(1)
  [0]=>
  array(2) {
    ["count"]=>
    int(0)
    ["dn"]=>
    NULL
  }
}
