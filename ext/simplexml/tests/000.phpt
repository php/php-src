--TEST--
SimpleXML: var_dump()
--SKIPIF--
<?php if (!extension_loaded("simplexml")) print "skip"; ?>
--FILE--
<?php

$sxe = simplexml_load_file(__DIR__.'/000.xml');

function test($what)
{
	global $sxe;
	echo "===$what\n";
	eval("var_dump(isset(\$$what));");
	eval("var_dump((bool)\$$what);");
	eval("if (isset(\$$what)) var_dump(count(\$$what));");
	eval("var_dump(\$$what);");
}

test('sxe');
test('sxe->elem1');
test('sxe->elem1[0]');
test('sxe->elem1[0]->elem2');
test('sxe->elem1[0]->elem2->bla');
test('sxe->elem1[0]["attr1"]');
test('sxe->elem1[0]->attr1');
test('sxe->elem1[1]');
test('sxe->elem1[2]');
test('sxe->elem11');
test('sxe->elem11->elem111');
test('sxe->elem11->elem111->elem1111');
test('sxe->elem22');
test('sxe->elem22->elem222');
test('sxe->elem22->attr22');
test('sxe->elem22["attr22"]');

?>
===DONE===
<?php exit(0); ?>
--EXPECTF--
===sxe
bool(true)
bool(true)
int(3)
object(SimpleXMLElement)#%d (3) {
  ["@attributes"]=>
  array(1) {
    ["id"]=>
    string(3) "123"
  }
  ["elem1"]=>
  array(2) {
    [0]=>
    string(36) "There is some text.Here is some more"
    [1]=>
    object(SimpleXMLElement)#%d (1) {
      ["@attributes"]=>
      array(2) {
        ["attr1"]=>
        string(2) "11"
        ["attr2"]=>
        string(2) "12"
      }
    }
  }
  ["elem11"]=>
  object(SimpleXMLElement)#%d (1) {
    ["elem111"]=>
    object(SimpleXMLElement)#%d (1) {
      ["elem1111"]=>
      object(SimpleXMLElement)#%d (0) {
      }
    }
  }
}
===sxe->elem1
bool(true)
bool(true)
int(2)
object(SimpleXMLElement)#%d (3) {
  ["@attributes"]=>
  array(2) {
    ["attr1"]=>
    string(5) "first"
    ["attr2"]=>
    string(6) "second"
  }
  ["comment"]=>
  object(SimpleXMLElement)#%d (0) {
  }
  ["elem2"]=>
  object(SimpleXMLElement)#%d (2) {
    ["@attributes"]=>
    array(2) {
      ["att25"]=>
      string(2) "25"
      ["att42"]=>
      string(2) "42"
    }
    ["elem3"]=>
    object(SimpleXMLElement)#%d (1) {
      ["elem4"]=>
      object(SimpleXMLElement)#%d (1) {
        ["test"]=>
        object(SimpleXMLElement)#%d (0) {
        }
      }
    }
  }
}
===sxe->elem1[0]
bool(true)
bool(true)
int(1)
object(SimpleXMLElement)#%d (3) {
  ["@attributes"]=>
  array(2) {
    ["attr1"]=>
    string(5) "first"
    ["attr2"]=>
    string(6) "second"
  }
  ["comment"]=>
  object(SimpleXMLElement)#%d (0) {
  }
  ["elem2"]=>
  object(SimpleXMLElement)#%d (2) {
    ["@attributes"]=>
    array(2) {
      ["att25"]=>
      string(2) "25"
      ["att42"]=>
      string(2) "42"
    }
    ["elem3"]=>
    object(SimpleXMLElement)#%d (1) {
      ["elem4"]=>
      object(SimpleXMLElement)#%d (1) {
        ["test"]=>
        object(SimpleXMLElement)#%d (0) {
        }
      }
    }
  }
}
===sxe->elem1[0]->elem2
bool(true)
bool(true)
int(1)
object(SimpleXMLElement)#%d (2) {
  ["@attributes"]=>
  array(2) {
    ["att25"]=>
    string(2) "25"
    ["att42"]=>
    string(2) "42"
  }
  ["elem3"]=>
  object(SimpleXMLElement)#%d (1) {
    ["elem4"]=>
    object(SimpleXMLElement)#%d (1) {
      ["test"]=>
      object(SimpleXMLElement)#%d (0) {
      }
    }
  }
}
===sxe->elem1[0]->elem2->bla
bool(false)
bool(false)
object(SimpleXMLElement)#%d (0) {
}
===sxe->elem1[0]["attr1"]
bool(true)
bool(true)
int(0)
object(SimpleXMLElement)#%d (1) {
  [0]=>
  string(5) "first"
}
===sxe->elem1[0]->attr1
bool(false)
bool(false)
object(SimpleXMLElement)#%d (0) {
}
===sxe->elem1[1]
bool(true)
bool(true)
int(0)
object(SimpleXMLElement)#%d (1) {
  ["@attributes"]=>
  array(2) {
    ["attr1"]=>
    string(2) "11"
    ["attr2"]=>
    string(2) "12"
  }
}
===sxe->elem1[2]
bool(false)
bool(false)
NULL
===sxe->elem11
bool(true)
bool(true)
int(1)
object(SimpleXMLElement)#%d (1) {
  ["elem111"]=>
  object(SimpleXMLElement)#%d (1) {
    ["elem1111"]=>
    object(SimpleXMLElement)#%d (0) {
    }
  }
}
===sxe->elem11->elem111
bool(true)
bool(true)
int(1)
object(SimpleXMLElement)#%d (1) {
  ["elem1111"]=>
  object(SimpleXMLElement)#%d (0) {
  }
}
===sxe->elem11->elem111->elem1111
bool(true)
bool(true)
int(1)
object(SimpleXMLElement)#%d (0) {
}
===sxe->elem22
bool(false)
bool(false)
object(SimpleXMLElement)#%d (0) {
}
===sxe->elem22->elem222
bool(false)
bool(false)
NULL
===sxe->elem22->attr22
bool(false)
bool(false)
NULL
===sxe->elem22["attr22"]
bool(false)
bool(false)
NULL
===DONE===
