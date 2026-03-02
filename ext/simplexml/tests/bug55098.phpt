--TEST--
Bug #55098 (SimpleXML iteration produces infinite loop)
--EXTENSIONS--
simplexml
--FILE--
<?php
$xmlString = "<root><a><b>1</b><b>2</b><b>3</b></a></root>";
$xml = simplexml_load_string($xmlString);

$nodes = $xml->a->b;

function test($nodes, $name, $callable) {
    echo "--- $name ---\n";
    foreach ($nodes as $nodeData) {
        echo "nodeData: " . $nodeData . "\n";
        $callable($nodes);
    }
}

test($nodes, "asXml", fn ($n) => $n->asXml());
test($nodes, "attributes", fn ($n) => $n->attributes());
test($nodes, "children", fn ($n) => $n->children());
test($nodes, "getNamespaces", fn ($n) => $n->getNamespaces());
test($nodes, "xpath", fn ($n) => $n->xpath("/root/a/b"));
test($nodes, "var_dump", fn ($n) => var_dump($n));
test($nodes, "manipulation combined with querying", function ($n) {
    $n->addAttribute("attr", "value");
    (bool) $n["attr"];
    $n->addChild("child", "value");
    $n->outer[]->inner = "foo";
    (bool) $n->outer;
    (bool) $n;
    isset($n->outer);
    isset($n["attr"]);
    unset($n->outer);
    unset($n["attr"]);
    unset($n->child);
});
?>
--EXPECT--
--- asXml ---
nodeData: 1
nodeData: 2
nodeData: 3
--- attributes ---
nodeData: 1
nodeData: 2
nodeData: 3
--- children ---
nodeData: 1
nodeData: 2
nodeData: 3
--- getNamespaces ---
nodeData: 1
nodeData: 2
nodeData: 3
--- xpath ---
nodeData: 1
nodeData: 2
nodeData: 3
--- var_dump ---
nodeData: 1
object(SimpleXMLElement)#3 (3) {
  [0]=>
  string(1) "1"
  [1]=>
  string(1) "2"
  [2]=>
  string(1) "3"
}
nodeData: 2
object(SimpleXMLElement)#3 (3) {
  [0]=>
  string(1) "1"
  [1]=>
  string(1) "2"
  [2]=>
  string(1) "3"
}
nodeData: 3
object(SimpleXMLElement)#3 (3) {
  [0]=>
  string(1) "1"
  [1]=>
  string(1) "2"
  [2]=>
  string(1) "3"
}
--- manipulation combined with querying ---
nodeData: 1
nodeData: 2
nodeData: 3
