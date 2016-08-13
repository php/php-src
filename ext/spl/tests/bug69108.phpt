--TEST--
Bug #69108 ("Segmentation fault" when (de)serializing SplObjectStorage)
--INI--
zend.enable_gc=1
--FILE--
<?php
$a = array();
$b = new SplObjectStorage();
for ($i = 10000; $i > 0; $i--) {
	    $object = new StdClass();
		    $a[] = $object;
		    $b->attach($object);
}

$c = serialize(array($a, $b));
$d = unserialize($c);

unset($d);
echo "ok";
?>
--EXPECT--
ok
