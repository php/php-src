--TEST--
Check libcurl features list
--SKIPIF--
<?php

include 'skipif.inc';

if (substr(PHP_OS, 0, 3) !== 'WIN') {
    exit('skip: test only works with Windows');
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
GSS-Negotiate => No
IDN => Yes
IPv6 => Yes
krb4 => No
Largefile => Yes
libz => Yes
NTLM => Yes
TrackMemory => No
NTLMWB => No
SPNEGO => Yes
SSL => Yes
SSPI => Yes
TLS-SRP => No
HTTP2 => Yes
GSSAPI => No
KERBEROS5 => Yes
UNIX_SOCKETS => No
PSL => No
HTTPS_PROXY => Yes
MULTI_SSL => %s
BROTLI => %s
Protocols => dict, file, ftp, ftps, gopher, http, https, imap, imaps, ldap, ldaps, pop3, pop3s, rtsp, scp, sftp, smb, smbs, smtp, smtps, telnet, tftp
Host => %s-pc-win32
SSL Version => OpenSSL/%s
ZLib Version => %s
libSSH Version => libssh2/%s
