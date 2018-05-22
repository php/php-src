--TEST--
Test setrawcookie to fail when output exists before
--CREDITS--
PHP TestFEst 2017 - PHPDublin, PHPSP - Joao P V Martins <jp.joao@gmail.com>
--FILE--
<?php
echo 'output' . PHP_EOL;
var_dump(@setrawcookie('cookie_name', rawurlencode('cookie_content')));
?>
--EXPECT--
output
bool(false)
