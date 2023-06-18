--TEST--
Bug #80332 (Completely broken array access functionality with DOMNamedNodeMap) - DOMNamedNodeMap variation
--EXTENSIONS--
dom
--FILE--
<?php

$doc = new DOMDocument;

$doc->loadHTML('<span attr1="value1" attr2="value2"></span>');

$x = new DOMXPath($doc);
$span = $x->query('//span')[0];

print "Node name: {$span->nodeName}\n";

function test($span, $key) {
    $key_formatted = match ($key) {
        false => 'false',
        true => 'true',
        null => 'null',
        default => is_string($key) ? "'$key'" : $key,
    };
    echo "Attribute [{$key_formatted}] name: ", $span->attributes[$key]->nodeName ?? '/', "\n";
    echo "Attribute [{$key_formatted}] value: ", $span->attributes[$key]->nodeValue ?? '/', "\n";
    echo "Attribute [{$key_formatted}] isset: ", isset($span->attributes[$key]) ? "yes" : "no", "\n";
    echo "\n";
}

test($span, 0);
test($span, false);
test($span, true);
test($span, null);
test($span, 'attr2');
// This one should fail because there is no 'hi' attribute
test($span, 'hi');
test($span, '0');
test($span, '0.5');
test($span, '1');
// This one should fail because it's out of bounds
test($span, '2147483647');

?>
--EXPECT--
Node name: span
Attribute [0] name: attr1
Attribute [0] value: value1
Attribute [0] isset: yes

Attribute [false] name: attr1
Attribute [false] value: value1
Attribute [false] isset: yes

Attribute [true] name: attr2
Attribute [true] value: value2
Attribute [true] isset: yes

Attribute [null] name: attr1
Attribute [null] value: value1
Attribute [null] isset: yes

Attribute ['attr2'] name: attr2
Attribute ['attr2'] value: value2
Attribute ['attr2'] isset: yes

Attribute ['hi'] name: /
Attribute ['hi'] value: /
Attribute ['hi'] isset: no

Attribute ['0'] name: attr1
Attribute ['0'] value: value1
Attribute ['0'] isset: yes

Attribute ['0.5'] name: attr1
Attribute ['0.5'] value: value1
Attribute ['0.5'] isset: yes

Attribute ['1'] name: attr2
Attribute ['1'] value: value2
Attribute ['1'] isset: yes

Attribute ['2147483647'] name: /
Attribute ['2147483647'] value: /
Attribute ['2147483647'] isset: no
