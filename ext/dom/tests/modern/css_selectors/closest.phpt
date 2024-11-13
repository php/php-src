--TEST--
Test DOM\Element::closest() method: legit cases
--EXTENSIONS--
dom
--FILE--
<?php

$xml = <<<XML
<root>
  <a/>
  <div class="foo" xml:id="div1">
    <div xml:id="div2">
      <div class="bar" xml:id="div3"/>
    </div>
  </div>
</root>
XML;

$dom = DOM\XMLDocument::createFromString($xml);

function test($el, $selector) {
  echo "--- Selector: $selector ---\n";
  var_dump($el->closest($selector)?->getAttribute('xml:id'));
}

test($dom->getElementById('div3'), 'div');
test($dom->getElementById('div3'), '[class="foo"]');
test($dom->getElementById('div3'), ':not(root)');
test($dom->getElementById('div3'), ':not(div)');
test($dom->getElementById('div3'), 'a');
test($dom->getElementById('div3'), 'root :not(div[class])');
test($dom->getElementById('div3'), 'root > :not(div[class])');

?>
--EXPECT--
--- Selector: div ---
string(4) "div3"
--- Selector: [class="foo"] ---
string(4) "div1"
--- Selector: :not(root) ---
string(4) "div3"
--- Selector: :not(div) ---
NULL
--- Selector: a ---
NULL
--- Selector: root :not(div[class]) ---
string(4) "div2"
--- Selector: root > :not(div[class]) ---
NULL
