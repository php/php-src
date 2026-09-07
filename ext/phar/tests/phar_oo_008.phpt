--TEST--
Phar object: iterating via SplFileObject
--EXTENSIONS--
phar
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

class MyFile extends SplFileObject
{
    function current(): string
    {
        return parent::fgets();
    }
}

$phar->setInfoClass('MyFile');
/** @var MyFile $v */
$v = $phar['a.csv'];

echo "===3===\n";
while(!$v->eof())
{
    echo $v->key() . "=>" . $v->fgets() . "\n";
}

echo "===4===\n";
$v->rewind();
while(!$v->eof())
{
    $l = $v->fgets();
    echo $v->key() . "=>" . $l . "\n";
}

echo "===5===\n";
foreach($v as $k => $d)
{
    echo "$k=>" . $d . "\n";
}

?>
--CLEAN--
<?php
unlink(__DIR__ . '/files/phar_oo_008.phar.php');
__halt_compiler();
?>
--EXPECT--
===1===
0=>1,2,3

1=>2,a,b

2=>3,"c","'e'"
===2===
0=>1,2,3
1=>2,a,b
2=>3,"c","'e'"
===3===
0=>1,2,3

1=>2,a,b

2=>3,"c","'e'"
===4===
1=>1,2,3

2=>2,a,b

3=>3,"c","'e'"
===5===
1=>1,2,3

3=>3,"c","'e'"
