--TEST--
Bug#43926 - isInstance() isn't equivalent to instanceof operator
--FILE--
<?php

class E {
}
class D extends E {
}

class A extends D {
}

class C extends A {
}

$ra = new ReflectionClass('A');
$rc = new ReflectionClass('C');
$rd = new ReflectionClass('D');
$re = new ReflectionClass('E');

$ca = $ra->newInstance();
$cc = $rc->newInstance();
$cd = $rd->newInstance();
$ce = $re->newInstance();

print("Is? A ". ($ra->isInstance($ca) ? 'true' : 'false') .", instanceof: ". (($ca instanceof A) ? 'true' : 'false') ."\n");
print("Is? C ". ($ra->isInstance($cc) ? 'true' : 'false') .", instanceof: ". (($ca instanceof C) ? 'true' : 'false') ."\n");
print("Is? D ". ($ra->isInstance($cd) ? 'true' : 'false') .", instanceof: ". (($ca instanceof D) ? 'true' : 'false') ."\n");
print("Is? E ". ($ra->isInstance($ce) ? 'true' : 'false') .", instanceof: ". (($ca instanceof E) ? 'true' : 'false') ."\n");

?>
--EXPECT--
Is? A true, instanceof: true
Is? C false, instanceof: false
Is? D true, instanceof: true
Is? E true, instanceof: true
--UEXPECT--
Is? A true, instanceof: true
Is? C false, instanceof: false
Is? D true, instanceof: true
Is? E true, instanceof: true