--TEST--
Bug #70345 (Multiple vulnerabilities related to PCRE functions)
--SKIPIF--
<?php
if (version_compare(PCRE_VERSION, '10.38', '>=')) {
    die("skip only for libpcre2 < 10.38");
}
?>
--FILE--
<?php
$regex = '/(?=xyz\K)/';
$subject = "aaaaxyzaaaa";

var_dump(preg_split($regex, $subject));

$regex = '/(a(?=xyz\K))/';
$subject = "aaaaxyzaaaa";
preg_match($regex, $subject, $matches);

var_dump($matches);
--EXPECTF--
bool(false)

Warning: preg_match(): Get subpatterns list failed in %s on line %d
array(0) {
}
