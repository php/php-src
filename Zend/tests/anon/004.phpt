--TEST--
testing anonymous inheritance
--FILE--
<?php
class Outer {
    private $dice;
    
    public function __construct($dice) {
        if ($dice instanceof Outer) {
            $this->dice = $dice->dice;
        } else $this->dice = $dice;
    }
    
    protected function getData() {
        return $this->dice;
    }
    
    public function getDataProxy() {
        return new class extends Outer {
            public function getProxyData() {
                return $this->getData();
            }
        } ($this);
    }
}
$outer = new Outer(array(rand(1,10)));
var_dump($outer->getDataProxy()->getProxyData());
?>
--EXPECTF--	
array(1) {
  [0]=>
  int(%d)
}


