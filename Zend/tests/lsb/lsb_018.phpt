--TEST--
ZE2 Late Static Binding and Singleton
--FILE--
<?php
abstract class Singleton
{
    static private $instances = array();
    static private $nextInstanceId = 0;
    private $instanceId = NULL;
    static final public function getInstance()
    {
        $caller = get_called_class();
        if (!isset(self::$instances[$caller])) {
            self::$instances[$caller] = new $caller;
            self::$instances[$caller]->instanceId = self::$nextInstanceId++;
        }
        return self::$instances[$caller];
    }
    public final function getInstanceId()
    {
        return $this->instanceId;
    }
    public final function identify()
    {
        var_dump($this);
    }
}

class Foo extends Singleton {
}

class Bar extends Singleton {
}

class Baz extends Bar {
}

$u = Foo::getInstance();
$v = Bar::getInstance();
$w = Baz::getInstance();

$u->identify();
$v->identify();
$w->identify();

$x = Foo::getInstance();
$y = Bar::getInstance();
$z = Baz::getInstance();

$u->identify();
$v->identify();
$w->identify();
$x->identify();
$y->identify();
$z->identify();
?>
--EXPECTF--
object(Foo)#%d (1) {
  ["instanceId":"Singleton":private]=>
  int(0)
}
object(Bar)#%d (1) {
  ["instanceId":"Singleton":private]=>
  int(1)
}
object(Baz)#%d (1) {
  ["instanceId":"Singleton":private]=>
  int(2)
}
object(Foo)#%d (1) {
  ["instanceId":"Singleton":private]=>
  int(0)
}
object(Bar)#%d (1) {
  ["instanceId":"Singleton":private]=>
  int(1)
}
object(Baz)#%d (1) {
  ["instanceId":"Singleton":private]=>
  int(2)
}
object(Foo)#%d (1) {
  ["instanceId":"Singleton":private]=>
  int(0)
}
object(Bar)#%d (1) {
  ["instanceId":"Singleton":private]=>
  int(1)
}
object(Baz)#%d (1) {
  ["instanceId":"Singleton":private]=>
  int(2)
}
