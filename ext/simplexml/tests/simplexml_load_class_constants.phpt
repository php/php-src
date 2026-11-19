--TEST--
simplexml_load_string(), simplexml_load_file() and simplexml_import_dom() resolve constants of the given class
--EXTENSIONS--
simplexml
dom
--INI--
opcache.enable_cli=1
--FILE--
<?php
const VALUE = 42;
const STR = "a";

class StringElement extends SimpleXMLElement {
    public $var = VALUE;
}
class FileElement extends SimpleXMLElement {
    public $var = VALUE;
}
class ImportedElement extends SimpleXMLElement {
    public $var = VALUE;
}
class UndefElement extends SimpleXMLElement {
    public $var = UNDEF_CONST;
}
class TypedElement extends SimpleXMLElement {
    public int $var = STR;
}
class ConstElement extends SimpleXMLElement {
    const C = UNDEF_CONST;
}

$file = __DIR__ . "/book.xml";
$dom = new DOMDocument();
$dom->loadXML("<root/>");

echo get_class(simplexml_load_string("<root/>", StringElement::class)), PHP_EOL;
echo get_class(simplexml_load_file($file, FileElement::class)), PHP_EOL;
echo get_class(simplexml_import_dom($dom, ImportedElement::class)), PHP_EOL;

$loaders = [
    fn($class) => simplexml_load_string("<root/>", $class),
    fn($class) => simplexml_load_file($file, $class),
    fn($class) => simplexml_import_dom($dom, $class),
];
foreach ($loaders as $load) {
    try {
        $load(UndefElement::class);
    } catch (Error $e) {
        echo $e::class, ": ", $e->getMessage(), PHP_EOL;
    }
}

try {
    simplexml_load_string("<root/>", TypedElement::class);
} catch (TypeError $e) {
    echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}

try {
    simplexml_load_string("<root/>", ConstElement::class);
} catch (Error $e) {
    echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}
?>
--EXPECT--
StringElement
FileElement
ImportedElement
Error: Undefined constant "UNDEF_CONST"
Error: Undefined constant "UNDEF_CONST"
Error: Undefined constant "UNDEF_CONST"
TypeError: Cannot assign string to property TypedElement::$var of type int
Error: Undefined constant "UNDEF_CONST"
