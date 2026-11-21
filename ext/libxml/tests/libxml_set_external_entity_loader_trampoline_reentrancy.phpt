--TEST--
libxml_set_external_entity_loader(): trampoline callback resetting the loader from within itself
--EXTENSIONS--
dom
--FILE--
<?php
class TrampolineTest {
    public function __call(string $name, array $arguments) {
        libxml_set_external_entity_loader(null);
        var_dump($this instanceof TrampolineTest);
        return 42;
    }
}

libxml_set_external_entity_loader([new TrampolineTest, 'entity_loader']);

$dd = new DOMDocument;
$dd->loadXML(<<<XML
<!DOCTYPE foo PUBLIC "-//FOO/BAR" "http://example.com/foobar">
<foo>bar</foo>
XML);
try {
    $dd->validate();
} catch (\Error $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
var_dump(libxml_get_external_entity_loader());
?>
--EXPECTF--
bool(true)

Warning: DOMDocument::validate(): I/O warning : failed to load external entity "42" in %s on line %d

Warning: DOMDocument::validate(): Could not load the external subset "http://example.com/foobar" in %s on line %d
NULL
