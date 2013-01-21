--TEST--
ZE2 Visibility of the property is bypassed and visibility of accessor is denied in subclass
--FILE--
<?php

class Foo {
    private $bar {
        private get {
            echo __METHOD__.'() = ';
            return $this->bar;
        }   
        private set {
            echo __METHOD__."($value)".PHP_EOL;
            $this->bar = $value;
        }   
    }   
    
    public function __construct() {
        echo __CLASS__.'::__construct()'.PHP_EOL;
        $this->bar = 20; 
    }   
    
    public function getbar() {
        return $this->bar;
    }   
}

class SubFoo extends Foo {
    public function getbar2() {
        return $this->bar;
    }   
}

$foo = new Foo();
#echo '$foo->bar = ';       var_dump($foo->bar);
echo '$foo->getbar() = ';   var_dump($foo->getbar());

echo PHP_EOL.'-------------------------'.PHP_EOL.PHP_EOL;

$subfoo = new SubFoo();
echo '$subfoo->getbar() = ';    var_dump($subfoo->getbar());
echo '$subfoo->getbar2() = ';   var_dump($subfoo->getbar2());

?>
--EXPECTF--
Foo::__construct()
Foo::$bar->set(20)
$foo->getbar() = Foo::$bar->get() = int(20)

-------------------------

Foo::__construct()
Foo::$bar->set(20)
$subfoo->getbar() = Foo::$bar->get() = int(20)
$subfoo->getbar2() = 
Fatal error: Call to private accessor Foo::$bar->get() from context 'SubFoo' in %s on line %d
