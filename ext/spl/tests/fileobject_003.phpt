--TEST--
SPL: SplFileInfo cloning
--FILE--
<?php

function test($name, $lc, $lp)
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
	
	$f = new SplFileObject($name);
	var_dump($name);
	var_dump($f->getPathName());
	$l = substr($f->getPathName(), -1);
	var_dump($l != '/' && $l != '\\' && $l == $lc);
	var_dump($f->getFileName());
	$l = substr($f->getFileName(), -1);
	var_dump($l != '/' && $l != '\\' && $l == $lc);
	var_dump($f->getPath());
	$l = substr($f->getPath(), -1);
	var_dump($l != '/' && $l != '\\' && $l == $lp);

	$fo = $o->openFile();
	var_dump($fo->getPathName(), $fo->getFileName(), $fo->getPath());
}

test(dirname(__FILE__) . '/' . 'fileobject_001a.txt', 't', substr(dirname(__FILE__),-1));
test(dirname(__FILE__) . '/', substr(dirname(__FILE__),-1), 'l');
test(dirname(__FILE__),       substr(dirname(__FILE__),-1), 'l');

?>
===DONE===
<?php exit(0); ?>
--EXPECTF--
===0===
object(SplFileInfo)#%d (2) {
  [u"pathName":u"SplFileInfo":private]=>
  unicode(%d) "%s"
  [u"fileName":u"SplFileInfo":private]=>
  unicode(%d) "fileobject_001a.txt"
}
object(SplFileInfo)#%d (2) {
  [u"pathName":u"SplFileInfo":private]=>
  unicode(%d) "%s"
  [u"fileName":u"SplFileInfo":private]=>
  unicode(%d) "fileobject_001a.txt"
}
bool(false)
bool(true)
bool(true)
%s(%d) "%sfileobject_001a.txt"
unicode(%d) "%sfileobject_001a.txt"
bool(true)
unicode(19) "fileobject_001a.txt"
bool(true)
string(%d) "%stests"
bool(true)
unicode(%d) "%sfileobject_001a.txt"
unicode(19) "fileobject_001a.txt"
unicode(%d) "%stests"
===1===
object(SplFileInfo)#%d (2) {
  [u"pathName":u"SplFileInfo":private]=>
  unicode(%d) "%s%etests"
  [u"fileName":u"SplFileInfo":private]=>
  unicode(%d) "tests"
}
object(SplFileInfo)#%d (2) {
  [u"pathName":u"SplFileInfo":private]=>
  unicode(%d) "%s"
  [u"fileName":u"SplFileInfo":private]=>
  unicode(%d) "tests"
}
bool(false)
bool(true)
bool(true)
%s(%d) "%stests/"
unicode(%d) "%stests"
bool(true)
unicode(5) "tests"
bool(true)
string(%d) "%sspl"
bool(true)
unicode(%d) "%stests"
unicode(%d) "tests"
unicode(%d) "%s%espl"
===2===
object(SplFileInfo)#%d (2) {
  [u"pathName":u"SplFileInfo":private]=>
  unicode(%d) "%s"
  [u"fileName":u"SplFileInfo":private]=>
  unicode(%d) "%s"
}
object(SplFileInfo)#%d (2) {
  [u"pathName":u"SplFileInfo":private]=>
  unicode(%d) "%s%etests"
  [u"fileName":u"SplFileInfo":private]=>
  unicode(%d) "%s"
}
bool(false)
bool(true)
bool(true)
%s(%d) "%stests"
unicode(%d) "%stests"
bool(true)
unicode(%d) "tests"
bool(true)
string(%d) "%sspl"
bool(true)
unicode(%d) "%stests"
unicode(5) "tests"
unicode(%d) "%sspl"
===DONE===
