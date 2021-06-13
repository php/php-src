--TEST--
Check for default OpenSSL config path on Windows
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) != 'WIN') {
    die('skip windows only test');
}
?>
--ENV--
OPENSSL_CONF=
--FILE--
<?php

ob_start();
phpinfo();
$info = ob_get_contents();
ob_end_clean();

preg_match(",Openssl default config [^ ]* (.*),", $info, $m);

if (isset($m[1])) {
    var_dump(str_replace('/', '\\', strtolower($m[1])));
} else {
    echo $info;
}
?>
--EXPECTF--
string(%d) "c:\program files%r( \(x86\)|)%r\common files\ssl\openssl.cnf"
