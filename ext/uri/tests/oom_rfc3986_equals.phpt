--TEST--
Uri\Rfc3986\Uri::equals() doesn't reach hard memory limit
--EXTENSIONS--
uri
--INI--
memory_limit=8M
memory_hard_limit_percent=125
--FILE--
<?php
$a = new Uri\Rfc3986\Uri("http://EXAMPLE.COM/" . str_repeat("a/../", 60000));
$b = clone $a;

$a->equals($b);

echo "not reached\n";
?>
--EXPECTF--
Fatal error: Allowed memory size of 8388608 bytes exhausted %S(tried to allocate %d bytes) in %s on line %d
