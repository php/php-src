--TEST--
Object serialization / unserialization: references amongst properties
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

echo "\n\n--- a refs b:\n";
$obj = new stdClass;
$obj->b = 1;
$obj->a = &$obj->b;
$obj->c = 1;
check($obj);

echo "\n\n--- a refs c:\n";
$obj = new stdClass;
$obj->c = 1;
$obj->a = &$obj->c;
$obj->b = 1;
check($obj);

echo "\n\n--- b refs a:\n";
$obj = new stdClass;
$obj->a = 1;
$obj->b = &$obj->a;
$obj->c = 1;
check($obj);

echo "\n\n--- b refs c:\n";
$obj = new stdClass;
$obj->a = 1;
$obj->c = 1;
$obj->b = &$obj->c;
check($obj);

echo "\n\n--- c refs a:\n";
$obj = new stdClass;
$obj->a = 1;
$obj->b = 1;
$obj->c = &$obj->a;
check($obj);

echo "\n\n--- c refs b:\n";
$obj = new stdClass;
$obj->a = 1;
$obj->b = 1;
$obj->c = &$obj->b;
check($obj);

echo "\n\n--- a,b refs c:\n";
$obj = new stdClass;
$obj->c = 1;
$obj->a = &$obj->c;
$obj->b = &$obj->c;
check($obj);

echo "\n\n--- a,c refs b:\n";
$obj = new stdClass;
$obj->b = 1;
$obj->a = &$obj->b;
$obj->c = &$obj->b;
check($obj);

echo "\n\n--- b,c refs a:\n";
$obj = new stdClass;
$obj->a = 1;
$obj->b = &$obj->a;
$obj->c = &$obj->a;
check($obj);

echo "Done";
?>
--EXPECTF--
--- a refs b:
object(stdClass)#%d (3) {
  ["b"]=>
  &int(1)
  ["a"]=>
  &int(1)
  ["c"]=>
  int(1)
}
string(55) "O:8:"stdClass":3:{s:1:"b";i:1;s:1:"a";R:2;s:1:"c";i:1;}"
object(stdClass)#%d (3) {
  ["b"]=>
  &int(1)
  ["a"]=>
  &int(1)
  ["c"]=>
  int(1)
}
object(stdClass)#%d (3) {
  ["b"]=>
  &string(14) "obj->a.changed"
  ["a"]=>
  &string(14) "obj->a.changed"
  ["c"]=>
  int(1)
}
object(stdClass)#%d (3) {
  ["b"]=>
  &string(14) "obj->b.changed"
  ["a"]=>
  &string(14) "obj->b.changed"
  ["c"]=>
  int(1)
}
object(stdClass)#%d (3) {
  ["b"]=>
  &string(14) "obj->b.changed"
  ["a"]=>
  &string(14) "obj->b.changed"
  ["c"]=>
  string(14) "obj->c.changed"
}


--- a refs c:
object(stdClass)#%d (3) {
  ["c"]=>
  &int(1)
  ["a"]=>
  &int(1)
  ["b"]=>
  int(1)
}
string(55) "O:8:"stdClass":3:{s:1:"c";i:1;s:1:"a";R:2;s:1:"b";i:1;}"
object(stdClass)#%d (3) {
  ["c"]=>
  &int(1)
  ["a"]=>
  &int(1)
  ["b"]=>
  int(1)
}
object(stdClass)#%d (3) {
  ["c"]=>
  &string(14) "obj->a.changed"
  ["a"]=>
  &string(14) "obj->a.changed"
  ["b"]=>
  int(1)
}
object(stdClass)#%d (3) {
  ["c"]=>
  &string(14) "obj->a.changed"
  ["a"]=>
  &string(14) "obj->a.changed"
  ["b"]=>
  string(14) "obj->b.changed"
}
object(stdClass)#%d (3) {
  ["c"]=>
  &string(14) "obj->c.changed"
  ["a"]=>
  &string(14) "obj->c.changed"
  ["b"]=>
  string(14) "obj->b.changed"
}


--- b refs a:
object(stdClass)#%d (3) {
  ["a"]=>
  &int(1)
  ["b"]=>
  &int(1)
  ["c"]=>
  int(1)
}
string(55) "O:8:"stdClass":3:{s:1:"a";i:1;s:1:"b";R:2;s:1:"c";i:1;}"
object(stdClass)#%d (3) {
  ["a"]=>
  &int(1)
  ["b"]=>
  &int(1)
  ["c"]=>
  int(1)
}
object(stdClass)#%d (3) {
  ["a"]=>
  &string(14) "obj->a.changed"
  ["b"]=>
  &string(14) "obj->a.changed"
  ["c"]=>
  int(1)
}
object(stdClass)#%d (3) {
  ["a"]=>
  &string(14) "obj->b.changed"
  ["b"]=>
  &string(14) "obj->b.changed"
  ["c"]=>
  int(1)
}
object(stdClass)#%d (3) {
  ["a"]=>
  &string(14) "obj->b.changed"
  ["b"]=>
  &string(14) "obj->b.changed"
  ["c"]=>
  string(14) "obj->c.changed"
}


--- b refs c:
object(stdClass)#%d (3) {
  ["a"]=>
  int(1)
  ["c"]=>
  &int(1)
  ["b"]=>
  &int(1)
}
string(55) "O:8:"stdClass":3:{s:1:"a";i:1;s:1:"c";i:1;s:1:"b";R:3;}"
object(stdClass)#%d (3) {
  ["a"]=>
  int(1)
  ["c"]=>
  &int(1)
  ["b"]=>
  &int(1)
}
object(stdClass)#%d (3) {
  ["a"]=>
  string(14) "obj->a.changed"
  ["c"]=>
  &int(1)
  ["b"]=>
  &int(1)
}
object(stdClass)#%d (3) {
  ["a"]=>
  string(14) "obj->a.changed"
  ["c"]=>
  &string(14) "obj->b.changed"
  ["b"]=>
  &string(14) "obj->b.changed"
}
object(stdClass)#%d (3) {
  ["a"]=>
  string(14) "obj->a.changed"
  ["c"]=>
  &string(14) "obj->c.changed"
  ["b"]=>
  &string(14) "obj->c.changed"
}


--- c refs a:
object(stdClass)#%d (3) {
  ["a"]=>
  &int(1)
  ["b"]=>
  int(1)
  ["c"]=>
  &int(1)
}
string(55) "O:8:"stdClass":3:{s:1:"a";i:1;s:1:"b";i:1;s:1:"c";R:2;}"
object(stdClass)#%d (3) {
  ["a"]=>
  &int(1)
  ["b"]=>
  int(1)
  ["c"]=>
  &int(1)
}
object(stdClass)#%d (3) {
  ["a"]=>
  &string(14) "obj->a.changed"
  ["b"]=>
  int(1)
  ["c"]=>
  &string(14) "obj->a.changed"
}
object(stdClass)#%d (3) {
  ["a"]=>
  &string(14) "obj->a.changed"
  ["b"]=>
  string(14) "obj->b.changed"
  ["c"]=>
  &string(14) "obj->a.changed"
}
object(stdClass)#%d (3) {
  ["a"]=>
  &string(14) "obj->c.changed"
  ["b"]=>
  string(14) "obj->b.changed"
  ["c"]=>
  &string(14) "obj->c.changed"
}


--- c refs b:
object(stdClass)#%d (3) {
  ["a"]=>
  int(1)
  ["b"]=>
  &int(1)
  ["c"]=>
  &int(1)
}
string(55) "O:8:"stdClass":3:{s:1:"a";i:1;s:1:"b";i:1;s:1:"c";R:3;}"
object(stdClass)#%d (3) {
  ["a"]=>
  int(1)
  ["b"]=>
  &int(1)
  ["c"]=>
  &int(1)
}
object(stdClass)#%d (3) {
  ["a"]=>
  string(14) "obj->a.changed"
  ["b"]=>
  &int(1)
  ["c"]=>
  &int(1)
}
object(stdClass)#%d (3) {
  ["a"]=>
  string(14) "obj->a.changed"
  ["b"]=>
  &string(14) "obj->b.changed"
  ["c"]=>
  &string(14) "obj->b.changed"
}
object(stdClass)#%d (3) {
  ["a"]=>
  string(14) "obj->a.changed"
  ["b"]=>
  &string(14) "obj->c.changed"
  ["c"]=>
  &string(14) "obj->c.changed"
}


--- a,b refs c:
object(stdClass)#%d (3) {
  ["c"]=>
  &int(1)
  ["a"]=>
  &int(1)
  ["b"]=>
  &int(1)
}
string(55) "O:8:"stdClass":3:{s:1:"c";i:1;s:1:"a";R:2;s:1:"b";R:2;}"
object(stdClass)#%d (3) {
  ["c"]=>
  &int(1)
  ["a"]=>
  &int(1)
  ["b"]=>
  &int(1)
}
object(stdClass)#%d (3) {
  ["c"]=>
  &string(14) "obj->a.changed"
  ["a"]=>
  &string(14) "obj->a.changed"
  ["b"]=>
  &string(14) "obj->a.changed"
}
object(stdClass)#%d (3) {
  ["c"]=>
  &string(14) "obj->b.changed"
  ["a"]=>
  &string(14) "obj->b.changed"
  ["b"]=>
  &string(14) "obj->b.changed"
}
object(stdClass)#%d (3) {
  ["c"]=>
  &string(14) "obj->c.changed"
  ["a"]=>
  &string(14) "obj->c.changed"
  ["b"]=>
  &string(14) "obj->c.changed"
}


--- a,c refs b:
object(stdClass)#%d (3) {
  ["b"]=>
  &int(1)
  ["a"]=>
  &int(1)
  ["c"]=>
  &int(1)
}
string(55) "O:8:"stdClass":3:{s:1:"b";i:1;s:1:"a";R:2;s:1:"c";R:2;}"
object(stdClass)#%d (3) {
  ["b"]=>
  &int(1)
  ["a"]=>
  &int(1)
  ["c"]=>
  &int(1)
}
object(stdClass)#%d (3) {
  ["b"]=>
  &string(14) "obj->a.changed"
  ["a"]=>
  &string(14) "obj->a.changed"
  ["c"]=>
  &string(14) "obj->a.changed"
}
object(stdClass)#%d (3) {
  ["b"]=>
  &string(14) "obj->b.changed"
  ["a"]=>
  &string(14) "obj->b.changed"
  ["c"]=>
  &string(14) "obj->b.changed"
}
object(stdClass)#%d (3) {
  ["b"]=>
  &string(14) "obj->c.changed"
  ["a"]=>
  &string(14) "obj->c.changed"
  ["c"]=>
  &string(14) "obj->c.changed"
}


--- b,c refs a:
object(stdClass)#%d (3) {
  ["a"]=>
  &int(1)
  ["b"]=>
  &int(1)
  ["c"]=>
  &int(1)
}
string(55) "O:8:"stdClass":3:{s:1:"a";i:1;s:1:"b";R:2;s:1:"c";R:2;}"
object(stdClass)#%d (3) {
  ["a"]=>
  &int(1)
  ["b"]=>
  &int(1)
  ["c"]=>
  &int(1)
}
object(stdClass)#%d (3) {
  ["a"]=>
  &string(14) "obj->a.changed"
  ["b"]=>
  &string(14) "obj->a.changed"
  ["c"]=>
  &string(14) "obj->a.changed"
}
object(stdClass)#%d (3) {
  ["a"]=>
  &string(14) "obj->b.changed"
  ["b"]=>
  &string(14) "obj->b.changed"
  ["c"]=>
  &string(14) "obj->b.changed"
}
object(stdClass)#%d (3) {
  ["a"]=>
  &string(14) "obj->c.changed"
  ["b"]=>
  &string(14) "obj->c.changed"
  ["c"]=>
  &string(14) "obj->c.changed"
}
Done
