--TEST--
Test for function posix_getlogin() basic functionality
--CREDITS--
Rodrigo Prado de Jesus <royopa [at] gmail [dot] com>
Lucas de Oliveira <contato@deoliveiralucas.net>
Rogerio Prado de Jesus <rogeriopradoj@gmail.com>
Felipe Almeida <felipealmeida948@gmail.com>
Ariana Kataoka <ariskataoka@gmail.com>
Leonardo Gouveia <leogouv@gmail.com>
User Groups: PHPVale PHPSP PHPDF PHP Women
--SKIPIF--
<?php 
    if(!extension_loaded("posix")) die("skip - POSIX extension not loaded");
?>
--FILE--
<?php
$getpwuid = posix_getpwuid(posix_geteuid())['name'];
$getlogin = posix_getlogin();

if ($getlogin === false || $getlogin === $getpwuid) {
    echo "Success";
} else {
    echo "posix_getpwuid(): " . $getpwuid . "\n";
    echo "posix_getlogin(): " . $getlogin . "\n";
}
?>
--EXPECT--
Success
