--TEST--
Scoping in destructor call
--SKIPIF--
<?php if (version_compare(zend_version(), '2.0.0-dev', '<')) die('skip ZendEngine 2 needed'); ?>
--FILE--
<?php
        class T
        {
                private $var = array();

                public function add($a)
                {
                        array_push($this->var, $a);
                }

                public function __destruct()
                {
                        print_r($this->var);
                }
        }

        class TT extends T
        {
        }
        $t = new TT();
        $t->add("Hello");
        $t->add("World");
?>
--EXPECT--
Array
(
    [0] => Hello
    [1] => World
)
