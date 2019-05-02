--TEST--
Check libcurl features list
--SKIPIF--
<?php

include 'skipif.inc';

if (substr(PHP_OS, 0, 3) === 'WIN') {
    exit('skip: test doesn\'t work with Windows');
}

$version = curl_version();

if ($version['version_number'] < 0x071306) {
    exit('skip: test works only with curl >= 7.19.6');
}

?>
--FILE--
<?php
	ob_start();
	phpinfo();
	$s = ob_get_contents();
	ob_end_clean();
	preg_match('/curl\n\n(.+)\n\n/siU', $s, $m);

	echo $m[1], "\n";

?>
--EXPECTF--
cURL support => enabled
cURL Information => %s
Age => %d
Features
AsynchDNS => Yes
CharConv => No
Debug => No
GSS-Negotiate => Yes
IDN => Yes
IPv6 => Yes
krb4 => No
Largefile => Yes
libz => Yes
NTLM => Yes
TrackMemory => No
NTLMWB => Yes
SPNEGO => No
SSL => Yes
SSPI => No
TLS-SRP => Yes
HTTP2 => No
Protocols => dict, file, ftp, ftps, gopher, http, https, imap, imaps, ldap, ldaps, pop3, pop3s, rtmp, rtsp, smtp, smtps, telnet, tftp
Host => %s
SSL Version => GnuTLS/%s
ZLib Version => %s
