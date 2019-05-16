--TEST--
Test setrawcookie basic functionality
--CREDITS--
PHP TestFEst 2017 - PHPDublin, PHPSP - Joao P V Martins <jp.joao@gmail.com>
--FILE--
<?php
var_dump(setrawcookie('cookie_name', rawurlencode('cookie_content')));
?>
--EXPECT--
bool(true)
