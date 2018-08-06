--TEST--
Bug #49639: Test current, key, reset, next work with Iterator classes
--FILE--
<?php
class bug49369_iterator implements Iterator {
   private $data = [
       'one','two','three'
   ];
   private $index = 0;
   function current()
   {
       return $this->data[$this->index];
   }
   function key()
   {
       return "p".$this->index;
   }
   function next()
   {
       if (($this->index + 1) < count($this->data)) {
           $this->index++;
       }
   }
   function rewind()
   {
       $this->index = 0;
   }

   function valid()
   {
       return $this->index < count($this->data);
   }
}

echo "*** Testing current, key, reset, next with Iterator ***\n";

echo "\n-- Test old behavior works for current(null) --\n";
$ary = null;
var_dump( current($ary) );

echo "\n-- Test old behavior works for current(NonIteratorClass) --\n";
$ary = new stdClass;
$ary->id1 = 1;
$ary->id2 = 2;
var_dump( current($ary) );

$iter = new bug49369_iterator;
echo "\n-- Test current with custom iterator --\n";
var_dump( current($iter) );

echo "\n-- Test key with custom iterator --\n";
var_dump( key($iter) );

echo "\n-- Test next/key with custom iterator --\n";
var_dump( next($iter) );
var_dump( key($iter) );

echo "\n-- Test reset with custom iterator --\n";
var_dump( next($iter) );
var_dump( reset($iter) );
var_dump( key($iter) );
var_dump( current($iter) );

?>
===DONE===
<?php exit(0); ?>
--EXPECTF--
*** Testing current, key, reset, next with Iterator ***

-- Test old behavior works for current(null) --

Warning: current() expects parameter 1 to be array, null given in %s on line %d
NULL

-- Test old behavior works for current(NonIteratorClass) --
int(1)

-- Test current with custom iterator --
string(3) "one"

-- Test key with custom iterator --
string(2) "p0"

-- Test next/key with custom iterator --
string(3) "two"
string(2) "p1"

-- Test reset with custom iterator --
string(5) "three"
string(3) "one"
string(2) "p0"
string(3) "one"
===DONE===
