--TEST--
Cannot make hook explicitly final in final property
--FILE--
<?php

class Test {
    final public $prop {
        final get => $field;
    }
}

?>
===DONE===
--EXPECT--
===DONE===
