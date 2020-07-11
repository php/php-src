--TEST--
ZE2 Data corruption in __set
--FILE--
<?php
$f = 'c="foo"';
class foo {
        const foobar=1;
        public $pp = array('t'=>null);

        function bar() {
                echo $this->t ='f';
        }
        function __get($prop)
        {
                return $this->pp[$prop];
        }
        function __set($prop, $val)
        {
                echo "__set";
                $this->pp[$prop] = '';
        }
}
$f = new foo;
$f->bar();
?>
--EXPECT--
__setf
