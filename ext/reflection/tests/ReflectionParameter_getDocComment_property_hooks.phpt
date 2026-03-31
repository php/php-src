--TEST--
Test ReflectionParameter::getDocComment() usage for property with hook.
--INI--
opcache.save_comments=1
--FILE--
<?php

class A
{
     public function __construct(
          /** $foo */
          public string $foo {
               /** getter */
               get { return "foo"; }
               /** setter */
               set(string $value /** $value */) {}
          }
     ) {}
}

$rc = new ReflectionClass('A');
$rp = $rc->getProperty('foo');
echo "\n---> Doc comment for A::property $" . $rp->getName() . ":\n";
var_dump($rp->getDocComment());

$rh = $rp->getHook(PropertyHookType::Get);
echo "\n---> Doc comment for A::property " . $rh->getName() . ":\n";
var_dump($rh->getDocComment());

$rh = $rp->getHook(PropertyHookType::Set);
echo "\n---> Doc comment for A::property " . $rh->getName() . ":\n";
var_dump($rh->getDocComment());

$rp = $rh->getParameters()[0];
echo "\n---> Doc comment for A::property \$foo::set parameter $" . $rp->getName() . ":\n";
var_dump($rp->getDocComment());

$rp = $rc->getConstructor()->getParameters()[0];
echo "\n---> Doc comment for A::constructor parameter $" . $rp->getName() . ":\n";
var_dump($rp->getDocComment());

?>
--EXPECTF--
---> Doc comment for A::property $foo:
string(%d) "/** $foo */"

---> Doc comment for A::property $foo::get:
string(%d) "/** getter */"

---> Doc comment for A::property $foo::set:
string(%d) "/** setter */"

---> Doc comment for A::property $foo::set parameter $value:
string(%d) "/** $value */"

---> Doc comment for A::constructor parameter $foo:
string(%d) "/** $foo */"
