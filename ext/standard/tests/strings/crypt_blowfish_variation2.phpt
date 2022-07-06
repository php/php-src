--TEST--
Test Blowfish crypt() does not fall back to DES when rounds are not specified,
or Blowfish is not available.
--FILE--
<?php
$crypt = crypt('U*U', '$2a$CCCCCCCCCCCCCCCCCCCCC.E5YPO9kmyuRGyh0XouQYb4YMJKvyOeW');
if ($crypt==='*0') {
    echo "OK\n";
} else {
    echo "Not OK\n";
}
?>
--EXPECT--
OK
