--TEST--
SPL: Validator
--FILE--
<?php
class Test implements Validator
{
    protected $messages = array();
    
    public function isValid($value)
    {
        if($value == 'test') {
            return true;
        }
        
        $this->messages[] = sprintf("Error : '%s' is not equal to 'test'", $value);
        
        return false;
    }
    
    public function getMessages()
    {
        return $this->messages;
    }
}

$test = new Test();

var_dump($test instanceof Validator) . PHP_EOL;
var_dump($test->isValid('test')) . PHP_EOL;
var_dump($test->isValid('test1')) . PHP_EOL;
var_dump($test->getMessages()) . PHP_EOL;
?>
--EXPECT--
bool(true)
bool(true)
bool(false)
array(1) {
  [0]=>
  string(38) "Error : 'test1' is not equal to 'test'"
}
