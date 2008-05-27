--TEST--
Object serialization / unserialization: properties reference containing object 
--INI--
error_reporting = E_ALL & ~E_STRICT
--FILE--
<?php

function check(&$obj) {
	var_dump($obj);
	$ser = serialize($obj);
	var_dump($ser);
	
	$uobj = unserialize($ser);
	var_dump($uobj);
	$uobj->a = "obj->a.changed";
	var_dump($uobj);
	$uobj->b = "obj->b.changed";
	var_dump($uobj);
	$uobj->c = "obj->c.changed";
	var_dump($uobj);	
}

echo "\n\n--- a refs container:\n";
$ext = 1;
$obj = new stdClass;
$obj->a = &$obj;
$obj->b = 1;
$obj->c = 1;
check($obj);

echo "\n\n--- a eqs container:\n";
$ext = 1;
$obj = new stdClass;
$obj->a = $obj;
$obj->b = 1;
$obj->c = 1;
check($obj);

echo "\n\n--- a,b ref container:\n";
$ext = 1;
$obj = new stdClass;
$obj->a = &$obj;
$obj->b = &$obj;
$obj->c = 1;
check($obj);

echo "\n\n--- a,b eq container:\n";
$ext = 1;
$obj = new stdClass;
$obj->a = $obj;
$obj->b = $obj;
$obj->c = 1;
check($obj);

echo "\n\n--- a,b,c ref container:\n";
$ext = 1;
$obj = new stdClass;
$obj->a = &$obj;
$obj->b = &$obj;
$obj->c = &$obj;
check($obj);

echo "\n\n--- a,b,c eq container:\n";
$ext = 1;
$obj = new stdClass;
$obj->a = $obj;
$obj->b = $obj;
$obj->c = $obj;
check($obj);

echo "Done";
?>
--EXPECTF--
--- a refs container:
object(stdClass)#%d (3) {
  [u"a"]=>
  &object(stdClass)#%d (3) {
    [u"a"]=>
    *RECURSION*
    [u"b"]=>
    int(1)
    [u"c"]=>
    int(1)
  }
  [u"b"]=>
  int(1)
  [u"c"]=>
  int(1)
}
unicode(55) "O:8:"stdClass":3:{U:1:"a";R:1;U:1:"b";i:1;U:1:"c";i:1;}"
object(stdClass)#%d (3) {
  [u"a"]=>
  object(stdClass)#%d (3) {
    [u"a"]=>
    *RECURSION*
    [u"b"]=>
    int(1)
    [u"c"]=>
    int(1)
  }
  [u"b"]=>
  int(1)
  [u"c"]=>
  int(1)
}
object(stdClass)#%d (3) {
  [u"a"]=>
  unicode(14) "obj->a.changed"
  [u"b"]=>
  int(1)
  [u"c"]=>
  int(1)
}
object(stdClass)#%d (3) {
  [u"a"]=>
  unicode(14) "obj->a.changed"
  [u"b"]=>
  unicode(14) "obj->b.changed"
  [u"c"]=>
  int(1)
}
object(stdClass)#%d (3) {
  [u"a"]=>
  unicode(14) "obj->a.changed"
  [u"b"]=>
  unicode(14) "obj->b.changed"
  [u"c"]=>
  unicode(14) "obj->c.changed"
}


--- a eqs container:
object(stdClass)#%d (3) {
  [u"a"]=>
  object(stdClass)#%d (3) {
    [u"a"]=>
    *RECURSION*
    [u"b"]=>
    int(1)
    [u"c"]=>
    int(1)
  }
  [u"b"]=>
  int(1)
  [u"c"]=>
  int(1)
}
unicode(55) "O:8:"stdClass":3:{U:1:"a";r:1;U:1:"b";i:1;U:1:"c";i:1;}"
object(stdClass)#%d (3) {
  [u"a"]=>
  object(stdClass)#%d (3) {
    [u"a"]=>
    *RECURSION*
    [u"b"]=>
    int(1)
    [u"c"]=>
    int(1)
  }
  [u"b"]=>
  int(1)
  [u"c"]=>
  int(1)
}
object(stdClass)#%d (3) {
  [u"a"]=>
  unicode(14) "obj->a.changed"
  [u"b"]=>
  int(1)
  [u"c"]=>
  int(1)
}
object(stdClass)#%d (3) {
  [u"a"]=>
  unicode(14) "obj->a.changed"
  [u"b"]=>
  unicode(14) "obj->b.changed"
  [u"c"]=>
  int(1)
}
object(stdClass)#%d (3) {
  [u"a"]=>
  unicode(14) "obj->a.changed"
  [u"b"]=>
  unicode(14) "obj->b.changed"
  [u"c"]=>
  unicode(14) "obj->c.changed"
}


--- a,b ref container:
object(stdClass)#%d (3) {
  [u"a"]=>
  &object(stdClass)#%d (3) {
    [u"a"]=>
    *RECURSION*
    [u"b"]=>
    *RECURSION*
    [u"c"]=>
    int(1)
  }
  [u"b"]=>
  &object(stdClass)#%d (3) {
    [u"a"]=>
    *RECURSION*
    [u"b"]=>
    *RECURSION*
    [u"c"]=>
    int(1)
  }
  [u"c"]=>
  int(1)
}
unicode(55) "O:8:"stdClass":3:{U:1:"a";R:1;U:1:"b";R:1;U:1:"c";i:1;}"
object(stdClass)#%d (3) {
  [u"a"]=>
  &object(stdClass)#%d (3) {
    [u"a"]=>
    *RECURSION*
    [u"b"]=>
    *RECURSION*
    [u"c"]=>
    int(1)
  }
  [u"b"]=>
  &object(stdClass)#%d (3) {
    [u"a"]=>
    *RECURSION*
    [u"b"]=>
    *RECURSION*
    [u"c"]=>
    int(1)
  }
  [u"c"]=>
  int(1)
}
object(stdClass)#%d (3) {
  [u"a"]=>
  &unicode(14) "obj->a.changed"
  [u"b"]=>
  &unicode(14) "obj->a.changed"
  [u"c"]=>
  int(1)
}
object(stdClass)#%d (3) {
  [u"a"]=>
  &unicode(14) "obj->b.changed"
  [u"b"]=>
  &unicode(14) "obj->b.changed"
  [u"c"]=>
  int(1)
}
object(stdClass)#%d (3) {
  [u"a"]=>
  &unicode(14) "obj->b.changed"
  [u"b"]=>
  &unicode(14) "obj->b.changed"
  [u"c"]=>
  unicode(14) "obj->c.changed"
}


--- a,b eq container:
object(stdClass)#%d (3) {
  [u"a"]=>
  object(stdClass)#%d (3) {
    [u"a"]=>
    *RECURSION*
    [u"b"]=>
    *RECURSION*
    [u"c"]=>
    int(1)
  }
  [u"b"]=>
  object(stdClass)#%d (3) {
    [u"a"]=>
    *RECURSION*
    [u"b"]=>
    *RECURSION*
    [u"c"]=>
    int(1)
  }
  [u"c"]=>
  int(1)
}
unicode(55) "O:8:"stdClass":3:{U:1:"a";r:1;U:1:"b";r:1;U:1:"c";i:1;}"
object(stdClass)#%d (3) {
  [u"a"]=>
  object(stdClass)#%d (3) {
    [u"a"]=>
    *RECURSION*
    [u"b"]=>
    *RECURSION*
    [u"c"]=>
    int(1)
  }
  [u"b"]=>
  object(stdClass)#%d (3) {
    [u"a"]=>
    *RECURSION*
    [u"b"]=>
    *RECURSION*
    [u"c"]=>
    int(1)
  }
  [u"c"]=>
  int(1)
}
object(stdClass)#%d (3) {
  [u"a"]=>
  unicode(14) "obj->a.changed"
  [u"b"]=>
  object(stdClass)#%d (3) {
    [u"a"]=>
    unicode(14) "obj->a.changed"
    [u"b"]=>
    *RECURSION*
    [u"c"]=>
    int(1)
  }
  [u"c"]=>
  int(1)
}
object(stdClass)#%d (3) {
  [u"a"]=>
  unicode(14) "obj->a.changed"
  [u"b"]=>
  unicode(14) "obj->b.changed"
  [u"c"]=>
  int(1)
}
object(stdClass)#%d (3) {
  [u"a"]=>
  unicode(14) "obj->a.changed"
  [u"b"]=>
  unicode(14) "obj->b.changed"
  [u"c"]=>
  unicode(14) "obj->c.changed"
}


--- a,b,c ref container:
object(stdClass)#%d (3) {
  [u"a"]=>
  &object(stdClass)#%d (3) {
    [u"a"]=>
    *RECURSION*
    [u"b"]=>
    *RECURSION*
    [u"c"]=>
    *RECURSION*
  }
  [u"b"]=>
  &object(stdClass)#%d (3) {
    [u"a"]=>
    *RECURSION*
    [u"b"]=>
    *RECURSION*
    [u"c"]=>
    *RECURSION*
  }
  [u"c"]=>
  &object(stdClass)#%d (3) {
    [u"a"]=>
    *RECURSION*
    [u"b"]=>
    *RECURSION*
    [u"c"]=>
    *RECURSION*
  }
}
unicode(55) "O:8:"stdClass":3:{U:1:"a";R:1;U:1:"b";R:1;U:1:"c";R:1;}"
object(stdClass)#%d (3) {
  [u"a"]=>
  &object(stdClass)#%d (3) {
    [u"a"]=>
    *RECURSION*
    [u"b"]=>
    *RECURSION*
    [u"c"]=>
    *RECURSION*
  }
  [u"b"]=>
  &object(stdClass)#%d (3) {
    [u"a"]=>
    *RECURSION*
    [u"b"]=>
    *RECURSION*
    [u"c"]=>
    *RECURSION*
  }
  [u"c"]=>
  &object(stdClass)#%d (3) {
    [u"a"]=>
    *RECURSION*
    [u"b"]=>
    *RECURSION*
    [u"c"]=>
    *RECURSION*
  }
}
object(stdClass)#%d (3) {
  [u"a"]=>
  &unicode(14) "obj->a.changed"
  [u"b"]=>
  &unicode(14) "obj->a.changed"
  [u"c"]=>
  &unicode(14) "obj->a.changed"
}
object(stdClass)#%d (3) {
  [u"a"]=>
  &unicode(14) "obj->b.changed"
  [u"b"]=>
  &unicode(14) "obj->b.changed"
  [u"c"]=>
  &unicode(14) "obj->b.changed"
}
object(stdClass)#%d (3) {
  [u"a"]=>
  &unicode(14) "obj->c.changed"
  [u"b"]=>
  &unicode(14) "obj->c.changed"
  [u"c"]=>
  &unicode(14) "obj->c.changed"
}


--- a,b,c eq container:
object(stdClass)#%d (3) {
  [u"a"]=>
  object(stdClass)#%d (3) {
    [u"a"]=>
    *RECURSION*
    [u"b"]=>
    *RECURSION*
    [u"c"]=>
    *RECURSION*
  }
  [u"b"]=>
  object(stdClass)#%d (3) {
    [u"a"]=>
    *RECURSION*
    [u"b"]=>
    *RECURSION*
    [u"c"]=>
    *RECURSION*
  }
  [u"c"]=>
  object(stdClass)#%d (3) {
    [u"a"]=>
    *RECURSION*
    [u"b"]=>
    *RECURSION*
    [u"c"]=>
    *RECURSION*
  }
}
unicode(55) "O:8:"stdClass":3:{U:1:"a";r:1;U:1:"b";r:1;U:1:"c";r:1;}"
object(stdClass)#%d (3) {
  [u"a"]=>
  object(stdClass)#%d (3) {
    [u"a"]=>
    *RECURSION*
    [u"b"]=>
    *RECURSION*
    [u"c"]=>
    *RECURSION*
  }
  [u"b"]=>
  object(stdClass)#%d (3) {
    [u"a"]=>
    *RECURSION*
    [u"b"]=>
    *RECURSION*
    [u"c"]=>
    *RECURSION*
  }
  [u"c"]=>
  object(stdClass)#%d (3) {
    [u"a"]=>
    *RECURSION*
    [u"b"]=>
    *RECURSION*
    [u"c"]=>
    *RECURSION*
  }
}
object(stdClass)#%d (3) {
  [u"a"]=>
  unicode(14) "obj->a.changed"
  [u"b"]=>
  object(stdClass)#%d (3) {
    [u"a"]=>
    unicode(14) "obj->a.changed"
    [u"b"]=>
    *RECURSION*
    [u"c"]=>
    *RECURSION*
  }
  [u"c"]=>
  object(stdClass)#%d (3) {
    [u"a"]=>
    unicode(14) "obj->a.changed"
    [u"b"]=>
    *RECURSION*
    [u"c"]=>
    *RECURSION*
  }
}
object(stdClass)#%d (3) {
  [u"a"]=>
  unicode(14) "obj->a.changed"
  [u"b"]=>
  unicode(14) "obj->b.changed"
  [u"c"]=>
  object(stdClass)#%d (3) {
    [u"a"]=>
    unicode(14) "obj->a.changed"
    [u"b"]=>
    unicode(14) "obj->b.changed"
    [u"c"]=>
    *RECURSION*
  }
}
object(stdClass)#%d (3) {
  [u"a"]=>
  unicode(14) "obj->a.changed"
  [u"b"]=>
  unicode(14) "obj->b.changed"
  [u"c"]=>
  unicode(14) "obj->c.changed"
}
Done
