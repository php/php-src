--TEST--
Test semi-reserved method and constant names and trait conflict resolution
--FILE--
<?php

trait TraitA
{
    public function catch(){ echo __METHOD__, PHP_EOL; }
    private function list(){ echo __METHOD__, PHP_EOL; }
}

trait TraitB
{
    static $list = ['a' => ['b' => ['c']]];

    public static function catch(){ echo __METHOD__, PHP_EOL; }
    private static function throw(){ echo __METHOD__, PHP_EOL; }
    private static function self(){ echo __METHOD__, PHP_EOL; }
}

trait TraitC
{
    public static function exit(){ echo __METHOD__, PHP_EOL; }
    protected static function try(){ echo __METHOD__, PHP_EOL; }
}

class Foo
{
    use TraitA, TraitB {
        TraitA
            ::
            catch insteadof namespace\TraitB;
        TraitA::list as public foreach;
        TraitB::throw as public;
        TraitB::self as public;
    }

    use TraitC {
        try as public attempt;
        exit as die;
        \TraitC::exit as bye;
        namespace\TraitC::exit as byebye;
        TraitC
            ::
            exit as farewell;
    }
}

(new Foo)->catch();
(new Foo)->foreach();
Foo::throw();
Foo::self();
var_dump(Foo::$list['a']);
Foo::attempt();
Foo::die();
Foo::bye();
Foo::byebye();
Foo::farewell();

echo "\nDone\n";

--EXPECTF--
TraitA::catch
TraitA::list
TraitB::throw
TraitB::self
array(1) {
  ["b"]=>
  array(1) {
    [0]=>
    string(1) "c"
  }
}
TraitC::try
TraitC::exit
TraitC::exit
TraitC::exit
TraitC::exit

Done
