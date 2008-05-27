--TEST--
SimpleXML: XPath and attributes
--SKIPIF--
<?php if (!extension_loaded("simplexml")) print "skip"; ?>
--FILE--
<?php

$xml =b<<<EOF
<?xml version='1.0'?>
<root>
<elem attr1='11' attr2='12' attr3='13'/>
<elem attr1='21' attr2='22' attr3='23'/>
<elem attr1='31' attr2='32' attr3='33'/>
</root>
EOF;

$sxe = simplexml_load_string($xml);

function test($xpath)
{
	global $sxe;

	echo "===$xpath===\n";
	var_dump($sxe->xpath($xpath));
}

test('elem/@attr2');
test('//@attr2');
test('//@*');
test('elem[2]/@attr2');

?>
===DONE===
--EXPECTF--
===elem/@attr2===
array(3) {
  [0]=>
  object(SimpleXMLElement)#%d (1) {
    [u"@attributes"]=>
    array(1) {
      [u"attr2"]=>
      unicode(2) "12"
    }
  }
  [1]=>
  object(SimpleXMLElement)#%d (1) {
    [u"@attributes"]=>
    array(1) {
      [u"attr2"]=>
      unicode(2) "22"
    }
  }
  [2]=>
  object(SimpleXMLElement)#%d (1) {
    [u"@attributes"]=>
    array(1) {
      [u"attr2"]=>
      unicode(2) "32"
    }
  }
}
===//@attr2===
array(3) {
  [0]=>
  object(SimpleXMLElement)#%d (1) {
    [u"@attributes"]=>
    array(1) {
      [u"attr2"]=>
      unicode(2) "12"
    }
  }
  [1]=>
  object(SimpleXMLElement)#%d (1) {
    [u"@attributes"]=>
    array(1) {
      [u"attr2"]=>
      unicode(2) "22"
    }
  }
  [2]=>
  object(SimpleXMLElement)#%d (1) {
    [u"@attributes"]=>
    array(1) {
      [u"attr2"]=>
      unicode(2) "32"
    }
  }
}
===//@*===
array(9) {
  [0]=>
  object(SimpleXMLElement)#%d (1) {
    [u"@attributes"]=>
    array(1) {
      [u"attr1"]=>
      unicode(2) "11"
    }
  }
  [1]=>
  object(SimpleXMLElement)#%d (1) {
    [u"@attributes"]=>
    array(1) {
      [u"attr2"]=>
      unicode(2) "12"
    }
  }
  [2]=>
  object(SimpleXMLElement)#%d (1) {
    [u"@attributes"]=>
    array(1) {
      [u"attr3"]=>
      unicode(2) "13"
    }
  }
  [3]=>
  object(SimpleXMLElement)#%d (1) {
    [u"@attributes"]=>
    array(1) {
      [u"attr1"]=>
      unicode(2) "21"
    }
  }
  [4]=>
  object(SimpleXMLElement)#%d (1) {
    [u"@attributes"]=>
    array(1) {
      [u"attr2"]=>
      unicode(2) "22"
    }
  }
  [5]=>
  object(SimpleXMLElement)#%d (1) {
    [u"@attributes"]=>
    array(1) {
      [u"attr3"]=>
      unicode(2) "23"
    }
  }
  [6]=>
  object(SimpleXMLElement)#%d (1) {
    [u"@attributes"]=>
    array(1) {
      [u"attr1"]=>
      unicode(2) "31"
    }
  }
  [7]=>
  object(SimpleXMLElement)#%d (1) {
    [u"@attributes"]=>
    array(1) {
      [u"attr2"]=>
      unicode(2) "32"
    }
  }
  [8]=>
  object(SimpleXMLElement)#%d (1) {
    [u"@attributes"]=>
    array(1) {
      [u"attr3"]=>
      unicode(2) "33"
    }
  }
}
===elem[2]/@attr2===
array(1) {
  [0]=>
  object(SimpleXMLElement)#%d (1) {
    [u"@attributes"]=>
    array(1) {
      [u"attr2"]=>
      unicode(2) "22"
    }
  }
}
===DONE===
