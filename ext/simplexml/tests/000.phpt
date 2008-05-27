--TEST--
SimpleXML: var_dump()
--SKIPIF--
<?php if (!extension_loaded("simplexml")) print "skip"; ?>
--FILE--
<?php 

$sxe = simplexml_load_file(dirname(__FILE__).'/000.xml');

function test($what)
{
	global $sxe;
	echo "===$what\n";
	eval("var_dump(isset(\$$what));");
	eval("var_dump((bool)\$$what);");
	eval("var_dump(count(\$$what));");
	eval("var_dump(\$$what);");
}

test('sxe');
test('sxe->elem1');
test('sxe->elem1[0]');
test('sxe->elem1[0]->elem2');
test('sxe->elem1[0]->elem2->bla');
if (!ini_get("unicode.semantics")) test('sxe->elem1[0]["attr1"]');
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
  [u"@attributes"]=>
  array(1) {
    [u"id"]=>
    unicode(3) "123"
  }
  [u"elem1"]=>
  array(2) {
    [0]=>
    unicode(36) "There is some text.Here is some more"
    [1]=>
    object(SimpleXMLElement)#%d (1) {
      [u"@attributes"]=>
      array(2) {
        [u"attr1"]=>
        unicode(2) "11"
        [u"attr2"]=>
        unicode(2) "12"
      }
    }
  }
  [u"elem11"]=>
  object(SimpleXMLElement)#%d (1) {
    [u"elem111"]=>
    object(SimpleXMLElement)#%d (1) {
      [u"elem1111"]=>
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
  [u"@attributes"]=>
  array(2) {
    [u"attr1"]=>
    unicode(5) "first"
    [u"attr2"]=>
    unicode(6) "second"
  }
  [u"comment"]=>
  object(SimpleXMLElement)#%d (0) {
  }
  [u"elem2"]=>
  object(SimpleXMLElement)#%d (2) {
    [u"@attributes"]=>
    array(2) {
      [u"att25"]=>
      unicode(2) "25"
      [u"att42"]=>
      unicode(2) "42"
    }
    [u"elem3"]=>
    object(SimpleXMLElement)#%d (1) {
      [u"elem4"]=>
      object(SimpleXMLElement)#%d (1) {
        [u"test"]=>
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
  [u"@attributes"]=>
  array(2) {
    [u"attr1"]=>
    unicode(5) "first"
    [u"attr2"]=>
    unicode(6) "second"
  }
  [u"comment"]=>
  object(SimpleXMLElement)#%d (0) {
  }
  [u"elem2"]=>
  object(SimpleXMLElement)#%d (2) {
    [u"@attributes"]=>
    array(2) {
      [u"att25"]=>
      unicode(2) "25"
      [u"att42"]=>
      unicode(2) "42"
    }
    [u"elem3"]=>
    object(SimpleXMLElement)#%d (1) {
      [u"elem4"]=>
      object(SimpleXMLElement)#%d (1) {
        [u"test"]=>
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
  [u"@attributes"]=>
  array(2) {
    [u"att25"]=>
    unicode(2) "25"
    [u"att42"]=>
    unicode(2) "42"
  }
  [u"elem3"]=>
  object(SimpleXMLElement)#%d (1) {
    [u"elem4"]=>
    object(SimpleXMLElement)#%d (1) {
      [u"test"]=>
      object(SimpleXMLElement)#%d (0) {
      }
    }
  }
}
===sxe->elem1[0]->elem2->bla
bool(false)
bool(false)
int(0)
object(SimpleXMLElement)#%d (0) {
}
===sxe->elem1[0]->attr1
bool(false)
bool(false)
int(0)
object(SimpleXMLElement)#%d (0) {
}
===sxe->elem1[1]
bool(true)
bool(true)
int(0)
object(SimpleXMLElement)#%d (1) {
  [u"@attributes"]=>
  array(2) {
    [u"attr1"]=>
    unicode(2) "11"
    [u"attr2"]=>
    unicode(2) "12"
  }
}
===sxe->elem1[2]
bool(false)
bool(false)
int(0)
NULL
===sxe->elem11
bool(true)
bool(true)
int(1)
object(SimpleXMLElement)#%d (1) {
  [u"elem111"]=>
  object(SimpleXMLElement)#%d (1) {
    [u"elem1111"]=>
    object(SimpleXMLElement)#%d (0) {
    }
  }
}
===sxe->elem11->elem111
bool(true)
bool(true)
int(1)
object(SimpleXMLElement)#%d (1) {
  [u"elem1111"]=>
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
int(0)
object(SimpleXMLElement)#%d (0) {
}
===sxe->elem22->elem222
bool(false)
bool(false)
int(0)
NULL
===sxe->elem22->attr22
bool(false)
bool(false)
int(0)
NULL
===sxe->elem22["attr22"]
bool(false)
bool(false)
int(0)
NULL
===DONE===
