--TEST--
ocipasswordchange()
--SKIPIF--
<?php if (!extension_loaded('oci8')) die("skip no oci8 extension"); ?>
--ENV--
return "
ORACLE_HOME=".(isset($_ENV['ORACLE_HOME']) ? $_ENV['ORACLE_HOME'] : '')."
NLS_LANG=".(isset($_ENV['NLS_LANG']) ? $_ENV['NLS_LANG'] : '')."
";
--FILE--
<?php

require dirname(__FILE__)."/connect.inc";

$new_password = "test";
var_dump(ocipasswordchange($c, $user, $password, $new_password));

if (!empty($dbase)) {
	var_dump($new_c = ocilogon($user,$new_password,$dbase));
}
else {
	var_dump($new_c = ocilogon($user,$new_password));
}

var_dump(ocipasswordchange($new_c, $user, $new_password, $password));


echo "Done\n";

?>
--EXPECTF--
bool(true)
resource(%d) of type (oci8 connection)
bool(true)
Done
