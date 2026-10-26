--TEST--
Uri\Rfc3986\Uri::withPath() doesn't reach hard memory limit
--EXTENSIONS--
uri
--INI--
memory_limit=8M
--FILE--
<?php
$uri = new Uri\Rfc3986\Uri("http://example.com/");

$uri->withPath("/" . str_repeat("a/", 300000));

echo "not reached\n";
?>
--EXPECTF--
Fatal error: Allowed memory size of 8388608 bytes exhausted %S(tried to allocate %d bytes) in %s on line %d
