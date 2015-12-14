--TEST--
Check for default OpenSSL config path on Windows
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) != 'WIN') {
    die('skip windows only test');
}
?>
--FILE--
<?php

ob_start();
phpinfo();
$info = ob_get_contents();
ob_end_clean();

preg_match(",Openssl default config .* (.*),", $info, $m);

if (isset($m[1])) {
	var_dump(str_replace('/', '\\', strtolower($m[1])));
}
?>
--EXPECT--
string(28) "c:\usr\local\ssl\openssl.cnf"
