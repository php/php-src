--TEST--
Bug #73092: Unserialize use-after-free when resizing object's properties hash table
--FILE--
<?php

class foo {
    function __wakeup() {
        $this->{'x'} = 1;
    }
}

unserialize('a:3:{i:0;O:3:"foo":8:{i:0;i:0;i:1;i:1;i:2;i:2;i:3;i:3;i:4;i:4;i:5;i:5;i:6;i:6;i:7;i:7;}i:1;s:263:"'.str_repeat("\06", 263).'";i:2;r:3;}');

?>
===DONE===
--EXPECT--
===DONE===
