--TEST--
GH-20479: Hooked object properties overflow
--CREDITS--
Viet Hoang Luu (@vi3tL0u1s)
--FILE--
<?php

#[AllowDynamicProperties]
class Trigger {
    public $a = 'x';
    public $b = 'x';
    public $c = 'x';
    public $d = 'x';
    public $e = 'x';
    public $f = 'x';
    public string $trigger {
      get {
        return 'trigger';
      }
    }
}

$obj = new Trigger();
// Add 2 dynamic props
$obj->g = $obj->h = 'x';
var_export($obj);

?>
--EXPECT--
\Trigger::__set_state(array(
   'a' => 'x',
   'b' => 'x',
   'c' => 'x',
   'd' => 'x',
   'e' => 'x',
   'f' => 'x',
   'trigger' => 'trigger',
   'h' => 'x',
   'g' => 'x',
))
