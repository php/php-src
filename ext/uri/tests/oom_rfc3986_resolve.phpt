--TEST--
Uri\Rfc3986\Uri::resolve() doesn't reach hard memory limit
--EXTENSIONS--
uri
--INI--
memory_limit=8M
--FILE--
<?php
$base = new Uri\Rfc3986\Uri("http://example.com/a/b");

$base->resolve(str_repeat("/", 1024 * 1024));

echo "not reached\n";
?>
--EXPECTF--
Fatal error: Allowed memory size of 8388608 bytes exhausted %S(tried to allocate %d bytes) in %s on line %d
