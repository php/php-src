--TEST--
Test Blowfish crypt() with invalid rounds
--SKIPIF--
<?php
if (!function_exists('crypt') || !defined("CRYPT_BLOWFISH")) {
    die("SKIP crypt()-blowfish is not available");
}
?>
--FILE--
<?php

$salts = array('32' => '$2a$32$CCCCCCCCCCCCCCCCCCCCCC$',
               '33' => '$2a$33$CCCCCCCCCCCCCCCCCCCCCC$',
               '34' => '$2a$34$CCCCCCCCCCCCCCCCCCCCCC$',
               '35' => '$2a$35$CCCCCCCCCCCCCCCCCCCCCC$',
               '36' => '$2a$36$CCCCCCCCCCCCCCCCCCCCCC$',
               '37' => '$2a$37$CCCCCCCCCCCCCCCCCCCCCC$',
               '38' => '$2a$38$CCCCCCCCCCCCCCCCCCCCCC$',);

foreach($salts as $i=>$salt) {
  $crypt = crypt('U*U', $salt);
  if ($crypt === '*0' || $crypt === '*1') {
    echo "$i. OK\n";
  } else {
    echo "$i. Not OK\n";
  }
}

?>
--EXPECT--
32. OK
33. OK
34. OK
35. OK
36. OK
37. OK
38. OK
