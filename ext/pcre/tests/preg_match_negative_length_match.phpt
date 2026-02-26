--TEST--
preg_match() resource cleanup when \K in lookahead causes negative-length match
--FILE--
<?php
$result = preg_match('/(?=ab\K)a/', 'ab', $matches);
var_dump($result);
?>
--EXPECTF--
Warning: preg_match(): Compilation failed: \K is not allowed in lookarounds (but see PCRE2_EXTRA_ALLOW_LOOKAROUND_BSK) at offset %d in %s
bool(false)
