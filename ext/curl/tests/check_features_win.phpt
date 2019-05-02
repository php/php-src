--TEST--
Check libcurl features list
--SKIPIF--
<?php

if (substr(PHP_OS, 0, 3) !== 'WIN') {
    exit('skip: test only works with Windows');
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
GSS-Negotiate => Yes
IDN => Yes
IPv6 => Yes
krb4 => No
Largefile => Yes
libz => Yes
NTLM => Yes
NTLMWB => No
SPNEGO => No
SSL => Yes
SSPI => Yes
TLS-SRP => No
TrackMemory => No
HTTP2 => Yes
GSSAPI => Yes
KERBEROS5 => Yes
UNIX_SOCKETS => Yes
PSL => Yes
HTTPS_PROXY => Yes
MULTI_SSL => %s
BROTLI => %s
Protocols => dict, file, ftp, ftps, gopher, http, https, imap, imaps, ldap, ldaps, pop3, pop3s, rtmp, rtsp, smb, smbs, smtp, smtps, telnet, tftp
Host => %s-pc-win32
SSL Version => OpenSSL/%s
ZLib Version => %s
libSSH Version => libssh2/%s
