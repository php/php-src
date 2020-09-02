--TEST--
Hash: hash_file() function : basic functionality
--CREDITS--
Felix De Vliegher <felix.devliegher@gmail.com>
--FILE--
<?php
echo "*** Testing hash_file() : basic functionality ***\n";

// Set up file
$filename = 'hash_file_basic_example.txt';
file_put_contents( $filename, 'The quick brown fox jumped over the lazy dog.' );

var_dump( hash_file( 'md5', $filename ) );
var_dump( hash_file( 'sha1', $filename ) );
var_dump( hash_file( 'sha256', $filename ) );
var_dump( hash_file( 'sha512', $filename ) );

var_dump( base64_encode( hash_file( 'md5', $filename, true ) ) );

?>
--CLEAN--
<?php

$filename = 'hash_file_basic_example.txt';
unlink( $filename );

?>
--EXPECT--
*** Testing hash_file() : basic functionality ***
string(32) "5c6ffbdd40d9556b73a21e63c3e0e904"
string(40) "c0854fb9fb03c41cce3802cb0d220529e6eef94e"
string(64) "68b1282b91de2c054c36629cb8dd447f12f096d3e3c587978dc2248444633483"
string(128) "0a8c150176c2ba391d7f1670ef4955cd99d3c3ec8cf06198cec30d436f2ac0c9b64229b5a54bdbd5563160503ce992a74be528761da9d0c48b7c74627302eb25"
string(24) "XG/73UDZVWtzoh5jw+DpBA=="
