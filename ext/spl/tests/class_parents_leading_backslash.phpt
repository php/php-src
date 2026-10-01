--TEST--
class_parents()/class_implements()/class_uses() accept a leading backslash regardless of $autoload
--FILE--
<?php

namespace {
    interface Iface {}
    trait Tr {}
    class ParentC implements Iface {}
    class ChildC extends ParentC {
        use Tr;
    }
}

namespace MyNS {
    interface NSIface {}
    trait NSTr {}
    class NSParentC implements NSIface {}
    class NSChildC extends NSParentC {
        use NSTr;
    }
}

namespace {

// The no-autoload path missed the leading-backslash strip before the fix.
var_dump(class_parents('\ChildC', false));
var_dump(class_parents('\ChildC'));
var_dump(class_implements('\ParentC', false));
var_dump(class_uses('\ChildC', false));

// Same for namespaced classes.
var_dump(class_parents('\MyNS\NSChildC', false));
var_dump(class_parents('\MyNS\NSChildC'));
var_dump(class_implements('\MyNS\NSParentC', false));
var_dump(class_uses('\MyNS\NSChildC', false));

}
?>
--EXPECT--
array(1) {
  ["ParentC"]=>
  string(7) "ParentC"
}
array(1) {
  ["ParentC"]=>
  string(7) "ParentC"
}
array(1) {
  ["Iface"]=>
  string(5) "Iface"
}
array(1) {
  ["Tr"]=>
  string(2) "Tr"
}
array(1) {
  ["MyNS\NSParentC"]=>
  string(14) "MyNS\NSParentC"
}
array(1) {
  ["MyNS\NSParentC"]=>
  string(14) "MyNS\NSParentC"
}
array(1) {
  ["MyNS\NSIface"]=>
  string(12) "MyNS\NSIface"
}
array(1) {
  ["MyNS\NSTr"]=>
  string(9) "MyNS\NSTr"
}
