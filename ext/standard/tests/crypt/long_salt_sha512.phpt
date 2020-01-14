--TEST--
Long salts for SHA-512 in crypt()
--FILE--
<?php

$b = str_repeat("A", 124);
var_dump(crypt("A", "$6$" . $b));
$b = str_repeat("A", 125);
var_dump(crypt("A", "$6$" . $b));
$b = str_repeat("A", 4096);
var_dump(crypt("A", "$6$" . $b));

?>
--EXPECT--
string(106) "$6$AAAAAAAAAAAAAAAA$B3fmGfJot8sGk90x9VFPiiM7K46ol5zLmywjTZNzPkCALir2CVAvUKfQQLPJOOL0rSOHiXcqCJWEK4uXwdhaE0"
string(106) "$6$AAAAAAAAAAAAAAAA$B3fmGfJot8sGk90x9VFPiiM7K46ol5zLmywjTZNzPkCALir2CVAvUKfQQLPJOOL0rSOHiXcqCJWEK4uXwdhaE0"
string(106) "$6$AAAAAAAAAAAAAAAA$B3fmGfJot8sGk90x9VFPiiM7K46ol5zLmywjTZNzPkCALir2CVAvUKfQQLPJOOL0rSOHiXcqCJWEK4uXwdhaE0"
