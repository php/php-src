--TEST--
GH-25267 (NULL zend_string dereference in to_xml_any with integer key)
--EXTENSIONS--
soap
--CREDITS--
YuanchengJiang
--FILE--
<?php
class C extends SoapClient {
    function __doRequest($request, $location, $action, $version, $one_way = false, ?string $uriParserClass = null): string {
        preg_match('#<ns1:foo>(.*)</ns1:foo>#', $request, $m);
        echo $m[1], PHP_EOL;
        return "";
    }
}
$c = new C(null, ["location" => "test://", "uri" => "test://"]);

try {
    $c->foo(new SoapVar([new SoapVar("y", XSD_STRING), "a" => new SoapVar("x", XSD_STRING)], XSD_ANYXML));
} catch (SoapFault $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

$c->foo(new SoapVar([new SoapVar("y", XSD_STRING, nodeName: "b"), "a" => new SoapVar("x", XSD_STRING)], XSD_ANYXML));
$c->foo(new SoapVar(["<b>y</b>", "a" => new SoapVar("x", XSD_STRING)], XSD_ANYXML));
?>
--EXPECT--
SoapFault: SOAP-ERROR: Encoding: Element at index 0 of 'any' content has no name
<b xsi:type="xsd:string">y</b><a xsi:type="xsd:string">x</a>
<b>y</b><a xsi:type="xsd:string">x</a>
