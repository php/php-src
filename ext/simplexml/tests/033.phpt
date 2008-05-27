--TEST--
SimpleXML: casting instances
--SKIPIF--
<?php if (!extension_loaded("simplexml")) print "skip"; ?>
--FILE--
<?php

$xml =b<<<EOF
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

$foo = simplexml_load_string( b"<foo />" );
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
===DONE===
--EXPECTF--
bool(false)
bool(true)
int(0)
int(0)
float(0)
float(0)
unicode(0) ""
unicode(15) "
test
  
  
  
"
array(0) {
}
array(1) {
  [u"person"]=>
  array(3) {
    [0]=>
    object(SimpleXMLElement)#%d (1) {
      [u"@attributes"]=>
      array(1) {
        [u"name"]=>
        unicode(3) "Joe"
      }
    }
    [1]=>
    object(SimpleXMLElement)#%d (2) {
      [u"@attributes"]=>
      array(1) {
        [u"name"]=>
        unicode(4) "John"
      }
      [u"children"]=>
      object(SimpleXMLElement)#%d (1) {
        [u"person"]=>
        object(SimpleXMLElement)#%d (1) {
          [u"@attributes"]=>
          array(1) {
            [u"name"]=>
            unicode(3) "Joe"
          }
        }
      }
    }
    [2]=>
    object(SimpleXMLElement)#%d (1) {
      [u"@attributes"]=>
      array(1) {
        [u"name"]=>
        unicode(4) "Jane"
      }
    }
  }
}
object(SimpleXMLElement)#%d (0) {
}
object(SimpleXMLElement)#%d (1) {
  [u"person"]=>
  array(3) {
    [0]=>
    object(SimpleXMLElement)#%d (1) {
      [u"@attributes"]=>
      array(1) {
        [u"name"]=>
        unicode(3) "Joe"
      }
    }
    [1]=>
    object(SimpleXMLElement)#%d (2) {
      [u"@attributes"]=>
      array(1) {
        [u"name"]=>
        unicode(4) "John"
      }
      [u"children"]=>
      object(SimpleXMLElement)#%d (1) {
        [u"person"]=>
        object(SimpleXMLElement)#%d (1) {
          [u"@attributes"]=>
          array(1) {
            [u"name"]=>
            unicode(3) "Joe"
          }
        }
      }
    }
    [2]=>
    object(SimpleXMLElement)#%d (1) {
      [u"@attributes"]=>
      array(1) {
        [u"name"]=>
        unicode(4) "Jane"
      }
    }
  }
}
===DONE===
