--TEST--
preg_match_all() resource cleanup when \K in lookahead causes negative-length match
--FILE--
<?php
$result = preg_match_all('/(?=ab\K)a/', 'ab', $matches);
var_dump($result);
?>
--EXPECTF--
Warning: preg_match_all(): Compilation failed: \K is not allowed in lookarounds (but see PCRE2_EXTRA_ALLOW_LOOKAROUND_BSK) at offset %d in %s
bool(false)
