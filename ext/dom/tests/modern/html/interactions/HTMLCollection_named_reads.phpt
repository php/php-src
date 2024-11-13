--TEST--
HTMLCollection::namedItem() and dimension handling for named accesses
--EXTENSIONS--
dom
--FILE--
<?php

$xml = <<<XML
<!DOCTYPE root [
    <!ENTITY ent "test">
]>
<root>
    <node name="foo">1</node>
    <x id="foo">2</x>
    <x id="foo&ent;">2 with entity</x>
    <node test:id="foo" xmlns:test="http://example.com">3</node>
    <node id="wrong">4</node>
    <node id="foo">5</node>
    <node name="bar">without html ns</node>
    <node name="bar" xmlns="http://www.w3.org/1999/xhtml">with html ns</node>
</root>
XML;

$dom = Dom\XMLDocument::createFromString($xml);

function test($obj, $name) {
    echo "--- Query \"$name\" ---\n";
    var_dump($obj->namedItem($name)?->textContent);
    var_dump($obj[$name]?->textContent);
    var_dump(isset($obj[$name]));

    // Search to check for dimension access consistency
    $node = $obj[$name];
    if ($node) {
        $found = false;
        for ($i = 0; $i < $obj->length && !$found; $i++) {
            $found = $obj[$i] === $node;
        }
        if (!$found) {
            throw new Error('inconsistency in dimension access');
        }
    }
}

test($dom->getElementsByTagName('node'), 'foo');
test($dom->getElementsByTagName('node'), '');
test($dom->getElementsByTagName('node'), 'does not exist');
test($dom->getElementsByTagName('node'), 'wrong');
test($dom->getElementsByTagName('node'), 'bar');
test($dom->getElementsByTagName('x'), 'foo');
test($dom->getElementsByTagName('x'), 'footest');

?>
--EXPECT--
--- Query "foo" ---
string(1) "5"
string(1) "5"
bool(true)
--- Query "" ---
NULL
NULL
bool(false)
--- Query "does not exist" ---
NULL
NULL
bool(false)
--- Query "wrong" ---
string(1) "4"
string(1) "4"
bool(true)
--- Query "bar" ---
string(12) "with html ns"
string(12) "with html ns"
bool(true)
--- Query "foo" ---
string(1) "2"
string(1) "2"
bool(true)
--- Query "footest" ---
string(13) "2 with entity"
string(13) "2 with entity"
bool(true)
