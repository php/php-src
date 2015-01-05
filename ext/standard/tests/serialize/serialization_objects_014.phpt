--TEST--
Object serialization / unserialization: references to external values 
--INI--
error_reporting = E_ALL & ~E_STRICT
--FILE--
<?php
/* Prototype  : proto string serialize(mixed variable)
 * Description: Returns a string representation of variable (which can later be unserialized) 
 * Source code: ext/standard/var.c
 * Alias to functions: 
 */
/* Prototype  : proto mixed unserialize(string variable_representation)
 * Description: Takes a string representation of variable and recreates it 
 * Source code: ext/standard/var.c
 * Alias to functions: 
 */

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

echo "\n\n--- a refs external:\n";
$ext = 1;
$obj = new stdClass;
$obj->a = &$ext;
$obj->b = 1;
$obj->c = 1;
check($obj);

echo "\n\n--- b refs external:\n";
$ext = 1;
$obj = new stdClass;
$obj->a = 1;
$obj->b = &$ext;
$obj->c = 1;
check($obj);

echo "\n\n--- c refs external:\n";
$ext = 1;
$obj = new stdClass;
$obj->a = 1;
$obj->b = 1;
$obj->c = &$ext;
check($obj);

echo "\n\n--- a,b ref external:\n";
$ext = 1;
$obj = new stdClass;
$obj->a = &$ext;
$obj->b = &$ext;
$obj->c = 1;
check($obj);

echo "\n\n--- a,b,c ref external:\n";
$ext = 1;
$obj = new stdClass;
$obj->a = &$ext;
$obj->b = &$ext;
$obj->c = &$ext;
check($obj);

echo "Done";
?>
--EXPECTF--

--- a refs external:
object(stdClass)#%d (3) {
  ["a"]=>
  &int(1)
  ["b"]=>
  int(1)
  ["c"]=>
  int(1)
}
string(55) "O:8:"stdClass":3:{s:1:"a";i:1;s:1:"b";i:1;s:1:"c";i:1;}"
object(stdClass)#%d (3) {
  ["a"]=>
  int(1)
  ["b"]=>
  int(1)
  ["c"]=>
  int(1)
}
object(stdClass)#%d (3) {
  ["a"]=>
  string(14) "obj->a.changed"
  ["b"]=>
  int(1)
  ["c"]=>
  int(1)
}
object(stdClass)#%d (3) {
  ["a"]=>
  string(14) "obj->a.changed"
  ["b"]=>
  string(14) "obj->b.changed"
  ["c"]=>
  int(1)
}
object(stdClass)#%d (3) {
  ["a"]=>
  string(14) "obj->a.changed"
  ["b"]=>
  string(14) "obj->b.changed"
  ["c"]=>
  string(14) "obj->c.changed"
}


--- b refs external:
object(stdClass)#%d (3) {
  ["a"]=>
  int(1)
  ["b"]=>
  &int(1)
  ["c"]=>
  int(1)
}
string(55) "O:8:"stdClass":3:{s:1:"a";i:1;s:1:"b";i:1;s:1:"c";i:1;}"
object(stdClass)#%d (3) {
  ["a"]=>
  int(1)
  ["b"]=>
  int(1)
  ["c"]=>
  int(1)
}
object(stdClass)#%d (3) {
  ["a"]=>
  string(14) "obj->a.changed"
  ["b"]=>
  int(1)
  ["c"]=>
  int(1)
}
object(stdClass)#%d (3) {
  ["a"]=>
  string(14) "obj->a.changed"
  ["b"]=>
  string(14) "obj->b.changed"
  ["c"]=>
  int(1)
}
object(stdClass)#%d (3) {
  ["a"]=>
  string(14) "obj->a.changed"
  ["b"]=>
  string(14) "obj->b.changed"
  ["c"]=>
  string(14) "obj->c.changed"
}


--- c refs external:
object(stdClass)#%d (3) {
  ["a"]=>
  int(1)
  ["b"]=>
  int(1)
  ["c"]=>
  &int(1)
}
string(55) "O:8:"stdClass":3:{s:1:"a";i:1;s:1:"b";i:1;s:1:"c";i:1;}"
object(stdClass)#%d (3) {
  ["a"]=>
  int(1)
  ["b"]=>
  int(1)
  ["c"]=>
  int(1)
}
object(stdClass)#%d (3) {
  ["a"]=>
  string(14) "obj->a.changed"
  ["b"]=>
  int(1)
  ["c"]=>
  int(1)
}
object(stdClass)#%d (3) {
  ["a"]=>
  string(14) "obj->a.changed"
  ["b"]=>
  string(14) "obj->b.changed"
  ["c"]=>
  int(1)
}
object(stdClass)#%d (3) {
  ["a"]=>
  string(14) "obj->a.changed"
  ["b"]=>
  string(14) "obj->b.changed"
  ["c"]=>
  string(14) "obj->c.changed"
}


--- a,b ref external:
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


--- a,b,c ref external:
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