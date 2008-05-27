--TEST--
Bug #43926 (isInstance() isn't equivalent to instanceof operator)
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
print("Is? C ". ($rc->isInstance($ca) ? 'true' : 'false') .", instanceof: ". (($ca instanceof C) ? 'true' : 'false') ."\n");
print("Is? D ". ($rd->isInstance($ca) ? 'true' : 'false') .", instanceof: ". (($ca instanceof D) ? 'true' : 'false') ."\n");
print("Is? E ". ($re->isInstance($ca) ? 'true' : 'false') .", instanceof: ". (($ca instanceof E) ? 'true' : 'false') ."\n");
print "-\n";
print("Is? A ". ($ra->isInstance($cc) ? 'true' : 'false') .", instanceof: ". (($cc instanceof A) ? 'true' : 'false') ."\n");
print("Is? C ". ($rc->isInstance($cc) ? 'true' : 'false') .", instanceof: ". (($cc instanceof C) ? 'true' : 'false') ."\n");
print("Is? D ". ($rd->isInstance($cc) ? 'true' : 'false') .", instanceof: ". (($cc instanceof D) ? 'true' : 'false') ."\n");
print("Is? E ". ($re->isInstance($cc) ? 'true' : 'false') .", instanceof: ". (($cc instanceof E) ? 'true' : 'false') ."\n");
print "-\n";
print("Is? A ". ($ra->isInstance($cd) ? 'true' : 'false') .", instanceof: ". (($cd instanceof A) ? 'true' : 'false') ."\n");
print("Is? C ". ($rc->isInstance($cd) ? 'true' : 'false') .", instanceof: ". (($cd instanceof C) ? 'true' : 'false') ."\n");
print("Is? D ". ($rd->isInstance($cd) ? 'true' : 'false') .", instanceof: ". (($cd instanceof D) ? 'true' : 'false') ."\n");
print("Is? E ". ($re->isInstance($cd) ? 'true' : 'false') .", instanceof: ". (($cd instanceof E) ? 'true' : 'false') ."\n");
print "-\n";
print("Is? A ". ($ra->isInstance($ce) ? 'true' : 'false') .", instanceof: ". (($ce instanceof A) ? 'true' : 'false') ."\n");
print("Is? C ". ($rc->isInstance($ce) ? 'true' : 'false') .", instanceof: ". (($ce instanceof C) ? 'true' : 'false') ."\n");
print("Is? D ". ($rd->isInstance($ce) ? 'true' : 'false') .", instanceof: ". (($ce instanceof D) ? 'true' : 'false') ."\n");
print("Is? E ". ($re->isInstance($ce) ? 'true' : 'false') .", instanceof: ". (($ce instanceof E) ? 'true' : 'false') ."\n");

?>
--EXPECT--
Is? A true, instanceof: true
Is? C false, instanceof: false
Is? D true, instanceof: true
Is? E true, instanceof: true
-
Is? A true, instanceof: true
Is? C true, instanceof: true
Is? D true, instanceof: true
Is? E true, instanceof: true
-
Is? A false, instanceof: false
Is? C false, instanceof: false
Is? D true, instanceof: true
Is? E true, instanceof: true
-
Is? A false, instanceof: false
Is? C false, instanceof: false
Is? D false, instanceof: false
Is? E true, instanceof: true
