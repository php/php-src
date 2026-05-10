--TEST--
Property substitution: get and set hook signatures are both substituted on a class extending a generic parent
--FILE--
<?php
class Box<T> {
    public T $val {
        get => $this->val;
        set => $this->val = $value;
    }
    public function __construct(T $v) { $this->val = $v; }
}

class IntBox extends Box<int> {}
class StrBox extends Box<string> {}

foreach (['IntBox', 'StrBox'] as $cn) {
    $rp = (new ReflectionClass($cn))->getProperty('val');
    $g = $rp->getHook(PropertyHookType::Get);
    $s = $rp->getHook(PropertyHookType::Set);
    echo "$cn: prop=", $rp->getType()->getName(),
         " get=", $g->getReturnType()->getName(),
         " set=", $s->getParameters()[0]->getType()->getName(), "\n";
}
?>
--EXPECT--
IntBox: prop=int get=int set=int
StrBox: prop=string get=string set=string
