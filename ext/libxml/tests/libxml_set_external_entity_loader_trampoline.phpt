--TEST--
libxml_set_external_entity_loader() trampoline callback
--EXTENSIONS--
dom
--FILE--
<?php
$xml = <<<XML
<!DOCTYPE foo PUBLIC "-//FOO/BAR" "http://example.com/foobar">
<foo>bar</foo>
XML;

class TrampolineTest {
    const DTD = '<!ELEMENT foo (#PCDATA)>';

    public function __call(string $name, array $arguments) {
        echo 'Trampoline for ', $name, PHP_EOL;
        var_dump($arguments);
        $f = fopen("php://temp", "r+");
        fwrite($f, self::DTD);
        rewind($f);
        return $f;
    }
}
$o = new TrampolineTest();
$callback = [$o, 'entity_loader'];

libxml_set_external_entity_loader($callback);

$dd = new DOMDocument;
$r = $dd->loadXML($xml);
var_dump($dd->validate());

echo "Done.\n";

?>
--EXPECT--
Trampoline for entity_loader
array(3) {
  [0]=>
  string(10) "-//FOO/BAR"
  [1]=>
  string(25) "http://example.com/foobar"
  [2]=>
  array(4) {
    ["directory"]=>
    NULL
    ["intSubName"]=>
    NULL
    ["extSubURI"]=>
    NULL
    ["extSubSystem"]=>
    NULL
  }
}
bool(true)
Done.
