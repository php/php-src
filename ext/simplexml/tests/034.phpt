--TEST--
SimpleXML: cast to array
--EXTENSIONS--
simplexml
--FILE--
<?php
$string = '<?xml version="1.0"?>
<foo><bar>
   <p>Blah 1</p>
   <p>Blah 2</p>
   <p>Blah 3</p>
   <tt>Blah 4</tt>
</bar></foo>
';
$foo = simplexml_load_string($string);
$p = $foo->bar->p;
echo count($p);
$p = (array)$foo->bar->p;
echo count($p);
?>
--EXPECT--
33
