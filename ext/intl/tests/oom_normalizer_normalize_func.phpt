--TEST--
normalizer_normalize() doesn't reach hard limit
--EXTENSIONS--
intl
--INI--
memory_limit=8M
--FILE--
<?php
$text = str_repeat("A", 2 * 1024 * 1024);
normalizer_normalize($text, Normalizer::FORM_D);
echo "not reached\n";
?>
--EXPECTF--
Fatal error: Allowed memory size of 8388608 bytes exhausted %S(tried to allocate %d bytes) in %s on line %d
