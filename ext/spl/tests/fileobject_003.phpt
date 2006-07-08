--TEST--
SPL: SplFileInfo cloning
--SKIPIF--
<?php if (!extension_loaded("spl")) print "skip"; ?>
--FILE--
<?php

function test($name)
{
	static $i = 0;
	echo "===$i===\n";
	$i++;

	$o = new SplFileInfo($name);

	var_dump($o);
	$c = clone $o;
	var_dump($c);
	var_dump($o === $c);
	var_dump($o == $c);
	var_dump($o->getPathname() == $c->getPathname());
}

test(dirname(__FILE__) . '/' . 'fileobject_001a.txt');
test(dirname(__FILE__) . '/');

?>
===DONE===
<?php exit(0); ?>
--EXPECTF--
===0===
object(SplFileInfo)#%d (0) {
}
object(SplFileInfo)#%d (0) {
}
bool(false)
bool(true)
bool(true)
===1===
object(SplFileInfo)#%d (0) {
}
object(SplFileInfo)#%d (0) {
}
bool(false)
bool(true)
bool(true)
===DONE===
