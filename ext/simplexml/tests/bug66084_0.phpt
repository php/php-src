--TEST--
Bug #66084 simplexml_load_string() mangles empty node name, var_dump variant
--EXTENSIONS--
simplexml
--FILE--
<?php
echo var_dump(simplexml_load_string('<a><b/><c><x/></c></a>')), "\n";
echo var_dump(simplexml_load_string('<a><b/><d/><c><x/></c></a>')), "\n";
echo var_dump(simplexml_load_string('<a><b/><c><d/><x/></c></a>')), "\n";
echo var_dump(simplexml_load_string('<a><b/><c><d><x/></d></c></a>')), "\n";
?>
--EXPECT--
object(SimpleXMLElement)#1 (2) {
  ["b"]=>
  object(SimpleXMLElement)#2 (0) {
  }
  ["c"]=>
  object(SimpleXMLElement)#3 (1) {
    ["x"]=>
    object(SimpleXMLElement)#4 (0) {
    }
  }
}

object(SimpleXMLElement)#1 (3) {
  ["b"]=>
  object(SimpleXMLElement)#3 (0) {
  }
  ["d"]=>
  object(SimpleXMLElement)#2 (0) {
  }
  ["c"]=>
  object(SimpleXMLElement)#4 (1) {
    ["x"]=>
    object(SimpleXMLElement)#5 (0) {
    }
  }
}

object(SimpleXMLElement)#1 (2) {
  ["b"]=>
  object(SimpleXMLElement)#4 (0) {
  }
  ["c"]=>
  object(SimpleXMLElement)#2 (2) {
    ["d"]=>
    object(SimpleXMLElement)#3 (0) {
    }
    ["x"]=>
    object(SimpleXMLElement)#5 (0) {
    }
  }
}

object(SimpleXMLElement)#1 (2) {
  ["b"]=>
  object(SimpleXMLElement)#2 (0) {
  }
  ["c"]=>
  object(SimpleXMLElement)#4 (1) {
    ["d"]=>
    object(SimpleXMLElement)#5 (1) {
      ["x"]=>
      object(SimpleXMLElement)#3 (0) {
      }
    }
  }
}
