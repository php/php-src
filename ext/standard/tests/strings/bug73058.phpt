--TEST--
Bug #73058 crypt broken when salt is 'too' long
--SKIPIF--
<?php
if (!function_exists('crypt'))) {
	die("SKIP crypt() is not available");
}
?>
--FILE--
<?php
$pass = 'secret';

$salt = '$2y$07$usesomesillystringforsalt$';
var_dump(crypt($pass, $salt));

$salt = '$2y$07$usesomesillystringforsaltzzzzzzzzzzzzz$';
var_dump(crypt($pass, $salt));

$salt = '$2y$07$usesomesillystringforx';
var_dump(crypt($pass, $salt));

?>
==OK==
--EXPECT--
string(60) "$2y$07$usesomesillystringforex.u2VJUMLRWaJNuw0Hu2FvCEimdeYVO"
string(60) "$2y$07$usesomesillystringforex.u2VJUMLRWaJNuw0Hu2FvCEimdeYVO"
string(60) "$2y$07$usesomesillystringforuw2Gm1ef7lMsvtzSK2p/14F0q1e8uOCO"
==OK==
