--TEST--
mcrypt_create_iv
https://bugs.php.net/bug.php?id=55169
--CREDIT--
Ryan Biesemeyer <ryan@yaauie.com>
--FILE--
<?php
for( $i=1; $i<=64; $i = $i*2 ){
  echo 'Input: '. $i . PHP_EOL;
  $random = mcrypt_create_iv( $i, MCRYPT_DEV_URANDOM );
  echo ' Length: ' . strlen( $random ) . PHP_EOL;
  echo ' Hex: '. bin2hex( $random ) . PHP_EOL;
  echo PHP_EOL;
}
?>
--EXPECTF--
Input: 1
 Length: 1
 Hex: %x

Input: 2
 Length: 2
 Hex: %x

Input: 4
 Length: 4
 Hex: %x

Input: 8
 Length: 8
 Hex: %x

Input: 16
 Length: 16
 Hex: %x

Input: 32
 Length: 32
 Hex: %x

Input: 64
 Length: 64
 Hex: %x
