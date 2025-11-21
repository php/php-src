--TEST--
Check libcurl config on windows
--EXTENSIONS--
curl
--SKIPIF--
<?php

if(substr(PHP_OS, 0, 3) != 'WIN' )
  die("skip for windows only");
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
DONE
--EXPECTF--
cURL support => enabled
cURL version => %s
Age => %d
Host => %s-pc-win32
Features => alt-svc, AsynchDNS, HSTS, HTTP2, HTTPS-proxy, IDN, IPv6, Kerberos, Largefile, libz, NTLM, SPNEGO, SSL, SSPI, threadsafe, UnixSockets
Protocols => dict, file, ftp, ftps, gopher, %r(gophers, )?%rhttp, https, imap, imaps, ldap, ldaps, %r(mqtt, )?%rpop3, pop3s, rtsp, scp, sftp, smb, smbs, smtp, smtps, telnet, tftp%r(, ws)?(, wss)?%r
SSL Version => OpenSSL/%s
SSL backends => %s
Zlib Version => %s
Libssh Version => libssh2/%s
Nghttp2 Version => %s
DONE
