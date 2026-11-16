--TEST--
mb_parse_str() doesn't reach hard limit - variable registration
--EXTENSIONS--
mbstring
--INI--
memory_limit=8M
max_input_vars=10000000
--FILE--
<?php
/* 100000 variables, under 1 MB of input: the name/value tables fit, but
 * registering every variable does not. */
$query = '';
for ($i = 0; $i < 100000; $i++) {
    $query .= ($i ? '&' : '') . "k$i=v";
}

mb_parse_str($query, $result);

echo "not reached\n";
?>
--EXPECTF--
Fatal error: Allowed memory size of 8388608 bytes exhausted %S(tried to allocate %d bytes) in %s on line %d
