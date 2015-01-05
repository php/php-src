--TEST--
Bug #37947 (zend_ptr_stack reallocation problem)
--INI--
error_reporting=0
--FILE--
<?php
class test {
        function extend_zend_ptr_stack($count,$a,$b,$c,$d,$e) {
                if ($count>0) $this->extend_zend_ptr_stack($count -
1,$a,$b,$c,$d,$e);
        }

        function __wakeup() {
                $this->extend_zend_ptr_stack(10,'a','b','c','d','e');
        }
}

$str='a:2:{i:0;O:4:"test":0:{}junk';
var_dump(unserialize($str));
--EXPECT--
bool(false)
