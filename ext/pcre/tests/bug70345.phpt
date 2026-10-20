--TEST--
Bug #70345 (Multiple vulnerabilities related to PCRE functions)
--SKIPIF--
<?php
if (PCRE_VERSION_MAJOR == 10 && PCRE_VERSION_MINOR < 38) {
    die("skip old pcre version");
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
Warning: preg_split(): Compilation failed: \K is not allowed in lookarounds (but see PCRE2_EXTRA_ALLOW_LOOKAROUND_BSK) at offset 9 in %s on line %d
bool(false)

Warning: preg_match(): Compilation failed: \K is not allowed in lookarounds (but see PCRE2_EXTRA_ALLOW_LOOKAROUND_BSK) at offset 12 in %s on line %d
NULL
