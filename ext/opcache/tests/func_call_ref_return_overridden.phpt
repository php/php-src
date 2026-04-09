--TEST--
DO_UCALL must not be used for functions returning by reference
--DESCRIPTION--
The optimizer's zend_get_call_op() converts DO_FCALL to DO_UCALL for user
functions, but DO_UCALL hardcodes return_reference=0 in
i_init_func_execute_data(). When the called function returns by reference
(e.g. an overridden method using ASSIGN_REF), this produces invalid opcode
sequences. The fix is either to not use DO_UCALL when the function has
ZEND_ACC_RETURN_REFERENCE, or to make DO_UCALL honor it.
--FILE--
<?php
class Base {
    protected function &getData(): array {
        $x = [];
        return $x;
    }

    public function process(): array {
        if ($data = &$this->getData() && !isset($data['key'])) {
            // unreachable
        }
        return $data;
    }
}

class Child extends Base {
    protected function &getData(): array {
        static $x = ['value' => 42];
        return $x;
    }
}

$child = new Child();
$result = $child->process();
var_dump($result);
?>
--EXPECT--
array(1) {
  ["value"]=>
  int(42)
}
