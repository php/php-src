--TEST--
Phar object: iterating via SplFileObject
--SKIPIF--
<?php if (!extension_loaded("phar")) print "skip"; ?>
--FILE--
<?php

$pharconfig = 1;

require_once 'phar_oo_test.inc';

$phar = new Phar($fname);
$phar->setFileClass('SplFileObject');

$f = $phar['a.csv'];
foreach($f as $k => $v)
{
	echo "$k=>$v\n";
}

$f->setFlags(SplFileObject::DROP_NEW_LINE);

foreach($f as $k => $v)
{
	echo "$k=>$v\n";
}

class MyCSVFile extends SplFileObject
{
	function current()
	{
		return parent::fgetcsv(',', '"');
	}
}

$phar->setFileClass('MyCSVFile');
$v = $phar['a.csv'];

while(!$v->eof())
{
	echo $v->key() . "=>" . join('|',$v->fgetcsv()) . "\n";
}

foreach($v as $k => $d)
{
	echo "$k=>" . join('|',$d) . "\n";
}

class MyCSVFile2 extends SplFileObject
{
	function getCurrentLine()
	{
		echo __METHOD__ . "\n";
		return parent::fgetcsv(',', '"');
	}
}

$phar->setFileClass('MyCSVFile2');
$v = $phar['a.csv'];

foreach($v as $k => $d)
{
	echo "$k=>" . join('|',$d) . "\n";
}

?>
===DONE===
--CLEAN--
<?php 
unlink(dirname(__FILE__) . '/phar_oo_test.phar.php');
__halt_compiler();
?>
--EXPECTF--
0=>1,2,3

1=>2,a,b

2=>3,"c","'e'"
0=>1,2,3
1=>2,a,b
2=>3,"c","'e'"
0=>1|2|3
1=>2|a|b
2=>3|c|'e'
1=>1|2|3
3=>2|a|b
5=>3|c|'e'
MyCSVFile2::getCurrentLine
1=>1|2|3
MyCSVFile2::getCurrentLine
3=>2|a|b
MyCSVFile2::getCurrentLine
5=>3|c|'e'
===DONE===
