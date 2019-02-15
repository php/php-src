--TEST--
Test for function posix_getlogin() basic functionality
--CREDITS--
Rodrigo Prado de Jesus <royopa [at] gmail [dot] com>
Lucas de Oliveira <contato@deoliveiralucas.net>
Rogerio Prado de Jesus <rogeriopradoj@gmail.com>
Felipe Almeida <felipealmeida948@gmail.com>
Ariana Kataoka <ariskataoka@gmail.com>
Leonardo Gouveia <leogouv@gmail.com>
User Groups: PHPVale
             PHPSP
             PHPDF
             PHP Women
Hangout presented by #phptestfestbrasil on 2017-11-14
https://www.youtube.com/watch?v=bBWkzZEadNo
--SKIPIF--
<?php 
    if(!extension_loaded("posix")) die("skip - POSIX extension not loaded");
?>
--FILE--
<?php
$username = posix_getpwuid(posix_geteuid())['name'];

if (posix_getlogin() == false) {
    $username = false;
}

var_dump($username == posix_getlogin());
?>
--EXPECT--
bool(true)
