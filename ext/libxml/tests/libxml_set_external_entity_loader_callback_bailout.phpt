--TEST--
libxml_set_external_entity_loader(): the in-callback state is reset when the callback bails out
--EXTENSIONS--
dom
--INI--
memory_limit=8M
--FILE--
<?php
register_shutdown_function(function () {
    try {
        libxml_set_external_entity_loader(null);
        echo 'loader reset', PHP_EOL;
    } catch (\Error $e) {
        echo $e::class, ': ', $e->getMessage(), PHP_EOL;
    }
});

$loader = function ($public, $system, $context) {
    echo 'in loader', PHP_EOL;
    $s = str_repeat('a', 1024 * 1024 * 512);
};

libxml_set_external_entity_loader($loader);

$xml = <<<XML
<!DOCTYPE foo PUBLIC "-//FOO/BAR" "http://example.com/foobar">
<foo>bar</foo>
XML;

$dd = new DOMDocument;
$dd->loadXML($xml);
$dd->validate();

echo 'not reached', PHP_EOL;
?>
--EXPECTF--
in loader

Fatal error: Allowed memory size of %d bytes exhausted (tried to allocate %d bytes) in %s on line %d
loader reset
