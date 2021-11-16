--TEST--
SimpleXML: casting instances
--EXTENSIONS--
simplexml
--FILE--
<?php

$xml =<<<EOF
<people>
test
  <person name="Joe"/>
  <person name="John">
    <children>
      <person name="Joe"/>
    </children>
  </person>
  <person name="Jane"/>
</people>
EOF;

$foo = simplexml_load_string( "<foo />" );
$people = simplexml_load_string($xml);

var_dump((bool)$foo);
var_dump((bool)$people);
var_dump((int)$foo);
var_dump((int)$people);
var_dump((double)$foo);
var_dump((double)$people);
var_dump((string)$foo);
var_dump((string)$people);
var_dump((array)$foo);
var_dump((array)$people);
var_dump((object)$foo);
var_dump((object)$people);

?>
--EXPECTF--
bool(false)
bool(true)
int(0)
int(0)
float(0)
float(0)
string(0) ""
string(15) "
test
  
  
  
"
array(0) {
}
array(1) {
  ["person"]=>
  array(3) {
    [0]=>
    object(SimpleXMLElement)#%d (1) {
      ["@attributes"]=>
      array(1) {
        ["name"]=>
        string(3) "Joe"
      }
    }
    [1]=>
    object(SimpleXMLElement)#%d (2) {
      ["@attributes"]=>
      array(1) {
        ["name"]=>
        string(4) "John"
      }
      ["children"]=>
      object(SimpleXMLElement)#%d (1) {
        ["person"]=>
        object(SimpleXMLElement)#%d (1) {
          ["@attributes"]=>
          array(1) {
            ["name"]=>
            string(3) "Joe"
          }
        }
      }
    }
    [2]=>
    object(SimpleXMLElement)#%d (1) {
      ["@attributes"]=>
      array(1) {
        ["name"]=>
        string(4) "Jane"
      }
    }
  }
}
object(SimpleXMLElement)#%d (0) {
}
object(SimpleXMLElement)#%d (1) {
  ["person"]=>
  array(3) {
    [0]=>
    object(SimpleXMLElement)#%d (1) {
      ["@attributes"]=>
      array(1) {
        ["name"]=>
        string(3) "Joe"
      }
    }
    [1]=>
    object(SimpleXMLElement)#%d (2) {
      ["@attributes"]=>
      array(1) {
        ["name"]=>
        string(4) "John"
      }
      ["children"]=>
      object(SimpleXMLElement)#%d (1) {
        ["person"]=>
        object(SimpleXMLElement)#%d (1) {
          ["@attributes"]=>
          array(1) {
            ["name"]=>
            string(3) "Joe"
          }
        }
      }
    }
    [2]=>
    object(SimpleXMLElement)#%d (1) {
      ["@attributes"]=>
      array(1) {
        ["name"]=>
        string(4) "Jane"
      }
    }
  }
}
