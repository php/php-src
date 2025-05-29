--TEST--
Bug #70345 (Multiple vulnerabilities related to PCRE functions)
--SKIPIF--
<?php
if (PCRE_VERSION_MAJOR != 10 || PCRE_VERSION_MINOR >= 38) {
    die("skip new pcre version");
}
--FILE--
<?php
$regex = '/(?=xyz\K)/';
$subject = "aaaaxyzaaaa";

var_dump(preg_split($regex, $subject));

$regex = '/(a(?=xyz\K))/';
$subject = "aaaaxyzaaaa";
preg_match($regex, $subject, $matches);

var_dump($matches);
?>
--EXPECTF--
bool(false)

Warning: preg_match(): Get subpatterns list failed in %s on line %d
array(0) {
}
