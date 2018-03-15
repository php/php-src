--TEST--
Ensure by value assignments leave temporaries on the stack, for all sorts of assignees.
--FILE--
<?php

function f() { return 0; }
$a[0][1] = 'good';
$a[1][1] = 'bad';

echo "\n" . '$i=f(): ';
echo $a[$i=f()][++$i];
unset($i);

echo "\n" . '$$x=f(): ';
$x='i';
echo $a[$$x=f()][++$$x];
unset($i, $x);

echo "\n" . '${\'i\'}=f(): ';
echo $a[${'i'}=f()][++${'i'}];
unset(${'i'});

echo "\n" . '$i[0]=f(): ';
echo $a[$i[0]=f()][++$i[0]];
unset($i);

echo "\n" . '$i[0][0]=f(): ';
echo $a[$i[0][0]=f()][++$i[0][0]];
unset($i);

echo "\n" . '$i->p=f(): ';
echo $a[$i->p=f()][++$i->p];
unset($i);

echo "\n" . '$i->p->q=f(): ';
echo $a[$i->p->q=f()][++$i->p->q];
unset($i);

echo "\n" . '$i->p[0]=f(): ';
echo $a[$i->p[0]=f()][++$i->p[0]];
unset($i);

echo "\n" . '$i->p[0]->p=f(): ';
echo $a[$i->p[0]->p=f()][++$i->p[0]->p];
unset($i);

Class C {
	static $p;
}

echo "\n" . 'C::$p=f(): ';
echo $a[C::$p=f()][++C::$p];

echo "\n" . 'C::$p[0]=f(): ';
C::$p = array();
echo $a[C::$p[0]=f()][++C::$p[0]];

echo "\n" . 'C::$p->q=f(): ';
C::$p = new stdclass;
echo $a[C::$p->q=f()][++C::$p->q];
?>
--EXPECTF--
$i=f(): good
$$x=f(): good
${'i'}=f(): good
$i[0]=f(): good
$i[0][0]=f(): good
$i->p=f(): 
Warning: Creating default object from empty value in %s on line %d
good
$i->p->q=f(): 
Warning: Creating default object from empty value in %s on line %d
good
$i->p[0]=f(): good
$i->p[0]->p=f(): 
Warning: Creating default object from empty value in %s on line %d
good
C::$p=f(): good
C::$p[0]=f(): good
C::$p->q=f(): good
