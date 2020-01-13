--TEST--
Test crypt() with salts of the various algorithms supported
--FILE--
<?php
echo "MD5: CRYPT_MD5" . \PHP_EOL;
var_dump(crypt('test', '$1$twelve12char'));
echo "BCRYPT: CRYPT_BLOWFISH" . \PHP_EOL;
echo "$2a$" . \PHP_EOL;
var_dump(crypt('test', '$2a$10$twenty.two22characters'));
echo "$2x$" . \PHP_EOL;
var_dump(crypt('test', '$2x$10$twenty.two22characters'));
echo "$2y$" . \PHP_EOL;
var_dump(crypt('test', '$2y$10$twenty.two22characters'));
echo "SHA-256: CRYPT_SHA256" . \PHP_EOL;
var_dump(crypt('test', '$5$sixteen.16.chars'));
echo "SHA-512: CRYPT_SHA512" . \PHP_EOL;
var_dump(crypt('test', '$6$sixteen.16.chars'));
 
?>
--EXPECT--
MD5: CRYPT_MD5
string(34) "$1$twelve12$HEx9Sbyp8WOEnuFFumQo3."
BCRYPT: CRYPT_BLOWFISH
$2a$
string(60) "$2a$10$twenty.two22charactere7yivlu/ZPDuxyAcZv9DDu3IFt.9ZaB2"
$2x$
string(60) "$2x$10$twenty.two22charactere7yivlu/ZPDuxyAcZv9DDu3IFt.9ZaB2"
$2y$
string(60) "$2y$10$twenty.two22charactere7yivlu/ZPDuxyAcZv9DDu3IFt.9ZaB2"
SHA-256: CRYPT_SHA256
string(63) "$5$sixteen.16.chars$Atu/um7bzN7wLD1BNGJHpsln5MbuilheBZWWNyHRZR7"
SHA-512: CRYPT_SHA512
string(106) "$6$sixteen.16.chars$er432qPjP7uB4ou2D7j./owghcmvWpXReMl5mdubBMP3.lK.foOOLJy8HEPXoeuBbtV4mYnNWrXxUGsBbPsNC0"
