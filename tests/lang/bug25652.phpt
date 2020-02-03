--TEST--
Bug #25652 (Calling Global functions dynamically fails from Class scope)
--FILE--
<?php

    function testfunc ($var) {
        echo "testfunc $var\n";
    }

    class foo {
        public $arr = array('testfunc');
        function bar () {
            $this->arr[0]('testvalue');
        }
    }

    $a = new foo ();
    $a->bar ();

?>
--EXPECT--
testfunc testvalue
