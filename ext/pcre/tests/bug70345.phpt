--TEST--
Bug #70345 (Multiple vulnerabilities related to PCRE functions)
--FILE--
<?php
$regex = '/(?=xyz\K)/';
$subject = "aaaaxyzaaaa";

$v = preg_split($regex, $subject);
print_r($v);

$regex = '/(a(?=xyz\K))/';
$subject = "aaaaxyzaaaa";
preg_match($regex, $subject, $matches);

var_dump($matches);
--EXPECTF--
Notice: preg_split(): PCRE error 1 in %s%ebug70345.php on line %d
Array
(
    [0] => aaaaxyzaaaa
)

Warning: preg_match(): Get subpatterns list failed in %s on line %d
array(0) {
}
