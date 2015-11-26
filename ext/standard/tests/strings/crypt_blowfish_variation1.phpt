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

$salts = array(b'32' => b'$2a$32$CCCCCCCCCCCCCCCCCCCCCC$',
               b'33' => b'$2a$33$CCCCCCCCCCCCCCCCCCCCCC$',
               b'34' => b'$2a$34$CCCCCCCCCCCCCCCCCCCCCC$',
               b'35' => b'$2a$35$CCCCCCCCCCCCCCCCCCCCCC$',
               b'36' => b'$2a$36$CCCCCCCCCCCCCCCCCCCCCC$',
               b'37' => b'$2a$37$CCCCCCCCCCCCCCCCCCCCCC$',
               b'38' => b'$2a$38$CCCCCCCCCCCCCCCCCCCCCC$',);

foreach($salts as $i=>$salt) {
  $crypt = crypt(b'U*U', $salt);
  if ($crypt === b'*0' || $crypt === b'*1') {
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
