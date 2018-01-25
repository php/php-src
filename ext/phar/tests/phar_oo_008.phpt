--TEST--
Phar object: iterating via SplFileObject
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
<?php if (!extension_loaded("spl")) die("skip SPL not available"); ?>
--INI--
phar.require_hash=0
--FILE--
<?php

$pharconfig = 1;

require_once 'files/phar_oo_test.inc';

$phar = new Phar($fname);
$phar->setInfoClass('SplFileObject');

$f = $phar['a.csv'];
echo "===1===\n";
foreach($f as $k => $v)
{
	echo "$k=>$v\n";
}

$f->setFlags(SplFileObject::DROP_NEW_LINE);

echo "===2===\n";
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

$phar->setInfoClass('MyCSVFile');
$v = $phar['a.csv'];

echo "===3===\n";
while(!$v->eof())
{
	echo $v->key() . "=>" . join('|',$v->fgetcsv()) . "\n";
}

echo "===4===\n";
$v->rewind();
while(!$v->eof())
{
	$l = $v->fgetcsv();
	echo $v->key() . "=>" . join('|',$l) . "\n";
}

echo "===5===\n";
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

$phar->setInfoClass('MyCSVFile2');
$v = $phar['a.csv'];

echo "===6===\n";
foreach($v as $k => $d)
{
	echo "$k=>" . join('|',$d) . "\n";
}

?>
===DONE===
--CLEAN--
<?php
unlink(dirname(__FILE__) . '/files/phar_oo_008.phar.php');
__halt_compiler();
?>
--EXPECTF--
===1===
0=>1,2,3

1=>2,a,b

2=>3,"c","'e'"
===2===
0=>1,2,3
1=>2,a,b
2=>3,"c","'e'"
===3===
0=>1|2|3
0=>2|a|b
1=>3|c|'e'
===4===
0=>1|2|3
1=>2|a|b
2=>3|c|'e'
===5===
0=>1|2|3
1=>2|a|b
2=>3|c|'e'
===6===
MyCSVFile2::getCurrentLine
1=>1|2|3
MyCSVFile2::getCurrentLine
3=>2|a|b
MyCSVFile2::getCurrentLine
5=>3|c|'e'
===DONE===
