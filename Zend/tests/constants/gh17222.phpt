--TEST--
GH-17222: __PROPERTY__ does not work in all constant expression contexts
--FILE--
<?php

#[Attribute]
class MyAttribute {
    public function __construct(public string $msg) {}
}

class Foo
{
    #[MyAttr(msg: __PROPERTY__)]
    public string $i = __PROPERTY__ {
        #[MyAttr(msg: __PROPERTY__)]
        get {
            var_dump(__PROPERTY__);
            return $this->i;
        }
        set (#[MyAttr(msg: __PROPERTY__)] string $v) {
            $this->i = $v;
        }
    }
}

$f = new Foo();
var_dump($f->i);

$r = new ReflectionClass(Foo::class);
$p = $r->getProperty('i');
var_dump($p->getAttributes()[0]->getArguments()['msg']);

$get = $p->getHook(PropertyHookType::Get);
var_dump($get->getAttributes()[0]->getArguments()['msg']);

$set = $p->getHook(PropertyHookType::Set);
var_dump($set->getParameters()[0]->getAttributes()[0]->getArguments()['msg']);

?>
--EXPECT--
string(1) "i"
string(1) "i"
string(1) "i"
string(1) "i"
string(1) "i"
