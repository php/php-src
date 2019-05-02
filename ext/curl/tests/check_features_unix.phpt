--TEST--
Check libcurl features list
--SKIPIF--
<?php

if (substr(PHP_OS, 0, 3) === 'WIN') {
    exit('skip: test doesn\'t work with Windows');
}

$version = curl_version();

if ($version['version_number'] < 0x071d00) {
    exit('skip: test works only with curl >= 7.29.0');
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
GSS-Negotiate => No
IDN => Yes
IPv6 => Yes
krb4 => No
Largefile => Yes
libz => Yes
NTLM => Yes
NTLMWB => Yes
SPNEGO => Yes
SSL => Yes
SSPI => No
TLS-SRP => Yes
TrackMemory => No
HTTP2 => Yes
GSSAPI => Yes
KERBEROS5 => Yes
UNIX_SOCKETS => Yes
PSL => Yes
HTTPS_PROXY => Yes
MULTI_SSL => No
BROTLI => No
Protocols => dict, file, ftp, ftps, gopher, http, https, imap, imaps, ldap, ldaps, pop3, pop3s, rtmp, rtsp, smb, smbs, smtp, smtps, telnet, tftp
Host => %s
SSL Version => GnuTLS/%s
ZLib Version => %s
