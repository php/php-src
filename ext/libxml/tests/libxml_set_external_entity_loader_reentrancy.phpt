--TEST--
libxml_set_external_entity_loader(): re-registering the loader from within its own callback is safe (no use-after-free)
--EXTENSIONS--
dom
--FILE--
<?php
class Loader {
    public function load($public, $system, $context) {
        /* Re-enter the setter while this callback (and its bound $this) is still
         * on the stack. The bound object is only kept alive by the global loader
         * fcc, so freeing it here used to trigger a use-after-free. The loader now
         * holds its own reference for the duration of the call, so this is safe. */
        libxml_set_external_entity_loader(null);
        /* $this must still be valid after the re-registration. */
        var_dump($this instanceof Loader);

        $f = fopen("php://temp", "r+");
        fwrite($f, "<!ELEMENT foo (#PCDATA)>");
        rewind($f);
        return $f;
    }
}

$xml = <<<XML
<!DOCTYPE foo PUBLIC "-//FOO/BAR" "http://example.com/foobar">
<foo>bar</foo>
XML;

/* Pass a fresh object as a non-closure callable: after this call returns, the
 * only reference to it is the one held by the global entity loader. */
libxml_set_external_entity_loader([new Loader, 'load']);

$dd = new DOMDocument;
$dd->loadXML($xml);
var_dump($dd->validate());

/* The callback reset the loader to null while it was running. */
var_dump(libxml_get_external_entity_loader());

echo "Done.\n";
?>
--EXPECT--
bool(true)
bool(true)
NULL
Done.
