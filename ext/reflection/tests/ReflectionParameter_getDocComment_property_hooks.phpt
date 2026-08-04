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
          },
          public string $bar /** $bar */ {
               /** getter */
               get { return "bar"; }
               /** setter */
               set(string $value /** $value */) {}
          },
          public string $baz {
               /** getter */
               get { return "foo"; }
               /** setter */
               set(string $value /** $value */) {}
          } /** $baz */,
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

// ---------

$rp = $rc->getConstructor()->getParameters()[1];
echo "\n---> Doc comment for A::constructor parameter $" . $rp->getName() . ":\n";
var_dump($rp->getDocComment());

$rp = $rc->getProperty('bar');
echo "\n---> Doc comment for A::property $" . $rp->getName() . ":\n";
var_dump($rp->getDocComment());

$rh = $rp->getHook(PropertyHookType::Get);
echo "\n---> Doc comment for A::property " . $rh->getName() . ":\n";
var_dump($rh->getDocComment());

$rh = $rp->getHook(PropertyHookType::Set);
echo "\n---> Doc comment for A::property " . $rh->getName() . ":\n";
var_dump($rh->getDocComment());

$rp = $rh->getParameters()[0];
echo "\n---> Doc comment for A::property \$bar::set parameter $" . $rp->getName() . ":\n";
var_dump($rp->getDocComment());

// ---------

$rp = $rc->getConstructor()->getParameters()[2];
echo "\n---> Doc comment for A::constructor parameter $" . $rp->getName() . ":\n";
var_dump($rp->getDocComment());

$rp = $rc->getProperty('baz');
echo "\n---> Doc comment for A::property $" . $rp->getName() . ":\n";
var_dump($rp->getDocComment());

$rh = $rp->getHook(PropertyHookType::Get);
echo "\n---> Doc comment for A::property " . $rh->getName() . ":\n";
var_dump($rh->getDocComment());

$rh = $rp->getHook(PropertyHookType::Set);
echo "\n---> Doc comment for A::property " . $rh->getName() . ":\n";
var_dump($rh->getDocComment());

$rp = $rh->getParameters()[0];
echo "\n---> Doc comment for A::property \$baz::set parameter $" . $rp->getName() . ":\n";
var_dump($rp->getDocComment());

$rp = $rc->getConstructor()->getParameters()[0];
echo "\n---> Doc comment for A::constructor parameter $" . $rp->getName() . ":\n";
var_dump($rp->getDocComment());

?>
--EXPECTF--
---> Doc comment for A::property $foo:
string(11) "/** $foo */"

---> Doc comment for A::property $foo::get:
string(13) "/** getter */"

---> Doc comment for A::property $foo::set:
string(13) "/** setter */"

---> Doc comment for A::property $foo::set parameter $value:
string(13) "/** $value */"

---> Doc comment for A::constructor parameter $bar:
string(11) "/** $bar */"

---> Doc comment for A::property $bar:
string(11) "/** $bar */"

---> Doc comment for A::property $bar::get:
string(13) "/** getter */"

---> Doc comment for A::property $bar::set:
string(13) "/** setter */"

---> Doc comment for A::property $bar::set parameter $value:
string(13) "/** $value */"

---> Doc comment for A::constructor parameter $baz:
bool(false)

---> Doc comment for A::property $baz:
bool(false)

---> Doc comment for A::property $baz::get:
string(13) "/** getter */"

---> Doc comment for A::property $baz::set:
string(13) "/** setter */"

---> Doc comment for A::property $baz::set parameter $value:
string(13) "/** $value */"

---> Doc comment for A::constructor parameter $foo:
string(11) "/** $foo */"
