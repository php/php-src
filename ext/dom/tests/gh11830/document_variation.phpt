--TEST--
GH-11830 (ParentNode methods should perform their checks upfront) - document variation
--EXTENSIONS--
dom
--FILE--
<?php
function test($method) {
    $otherDoc = new DOMDocument;
    $otherDoc->loadXML(<<<XML
    <?xml version="1.0"?>
    <other/>
    XML);

    $otherElement = $otherDoc->documentElement;

    $doc = new DOMDocument;
    $doc->loadXML(<<<XML
    <?xml version="1.0"?>
    <container>
        <alone/>
        <child><testelement/></child>
    </container>
    XML);

    $testElement = $doc->documentElement->firstElementChild->nextElementSibling->firstElementChild;

    try {
        $doc->documentElement->firstElementChild->$method($testElement, $otherElement);
    } catch (\DOMException $e) {
        echo $e->getMessage(), "\n";
    }

    echo $otherDoc->saveXML();
    echo $doc->saveXML();
}

test("prepend");
test("append");
test("before");
test("after");
test("replaceWith");
?>
--EXPECT--
Wrong Document Error
<?xml version="1.0"?>
<other/>
<?xml version="1.0"?>
<container>
    <alone/>
    <child/>
</container>
Wrong Document Error
<?xml version="1.0"?>
<other/>
<?xml version="1.0"?>
<container>
    <alone/>
    <child/>
</container>
Wrong Document Error
<?xml version="1.0"?>
<other/>
<?xml version="1.0"?>
<container>
    <alone/>
    <child/>
</container>
Wrong Document Error
<?xml version="1.0"?>
<other/>
<?xml version="1.0"?>
<container>
    <alone/>
    <child/>
</container>
Wrong Document Error
<?xml version="1.0"?>
<other/>
<?xml version="1.0"?>
<container>
    <alone/>
    <child/>
</container>
