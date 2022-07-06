--TEST--
Evaluation order during assignments.
--FILE--
<?php
$b = "bb";
$a = "aa";

function foo()
{
echo "Bad call\n";
}

function baa()
{
echo "Good call\n";
}

$bb = "baa";

$aa = "foo";

$c = ${$a=$b};

$c();

$a1 = array("dead","dead","dead");
$a2 = array("dead","dead","live");
$a3 = array("dead","dead","dead");

$a = array($a1,$a2,$a3);

function live()
{
echo "Good call\n";
}

function dead()
{
echo "Bad call\n";
}

$i = 0;

$a[$i=1][++$i]();

$a = -1;

function foo1()
{
  global $a;
  return ++$a;
}

$arr = array(array(0,0),0);

$brr = array(0,0,array(0,0,0,5),0);
$crr = array(0,0,0,0,array(0,0,0,0,0,10),0,0);

$arr[foo1()][foo1()] = $brr[foo1()][foo1()] +
                     $crr[foo1()][foo1()];

$val = $arr[0][1];
echo "Expect 15 and get...$val\n";

$x = array(array(0),0);
function mod($b)
{
global $x;
$x = $b;
return 0;
}

$x1 = array(array(1),1);
$x2 = array(array(2),2);
$x3 = array(array(3),3);
$bx = array(10);

$x[mod($x1)][mod($x2)] = $bx[mod($x3)];

// expecting 10,3

var_dump($x);
?>
--EXPECT--
Good call
Good call
Expect 15 and get...15
array(2) {
  [0]=>
  array(1) {
    [0]=>
    int(10)
  }
  [1]=>
  int(3)
}
