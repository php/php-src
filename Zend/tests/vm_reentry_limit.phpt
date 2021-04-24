--TEST--
VM reentry limit
--INI--
zend.vm_reentry_limit=20
--FILE--
<?php

class Test1 {
    public function __destruct() {
        new Test1;
    }
}

class Test2 {
    public function __clone() {
        clone $this;
    }
}

try {
    new Test1;
} catch (Error $e) {
    echo $e, "\n";
}

echo "\n";

try {
    clone new Test2;
} catch (Error $e) {
    echo $e, "\n";
}

?>
--EXPECTF--
Error: VM reentry limit of 20 reached. Infinite recursion? in %s:%d
Stack trace:
#0 %s(%d): Test1->__destruct()
#1 %s(%d): Test1->__destruct()
#2 %s(%d): Test1->__destruct()
#3 %s(%d): Test1->__destruct()
#4 %s(%d): Test1->__destruct()
#5 %s(%d): Test1->__destruct()
#6 %s(%d): Test1->__destruct()
#7 %s(%d): Test1->__destruct()
#8 %s(%d): Test1->__destruct()
#9 %s(%d): Test1->__destruct()
#10 %s(%d): Test1->__destruct()
#11 %s(%d): Test1->__destruct()
#12 %s(%d): Test1->__destruct()
#13 %s(%d): Test1->__destruct()
#14 %s(%d): Test1->__destruct()
#15 %s(%d): Test1->__destruct()
#16 %s(%d): Test1->__destruct()
#17 %s(%d): Test1->__destruct()
#18 %s(%d): Test1->__destruct()
#19 %s(%d): Test1->__destruct()
#20 %s(%d): Test1->__destruct()
#21 {main}

Error: VM reentry limit of 20 reached. Infinite recursion? in %s:%d
Stack trace:
#0 %s(%d): Test2->__clone()
#1 %s(%d): Test2->__clone()
#2 %s(%d): Test2->__clone()
#3 %s(%d): Test2->__clone()
#4 %s(%d): Test2->__clone()
#5 %s(%d): Test2->__clone()
#6 %s(%d): Test2->__clone()
#7 %s(%d): Test2->__clone()
#8 %s(%d): Test2->__clone()
#9 %s(%d): Test2->__clone()
#10 %s(%d): Test2->__clone()
#11 %s(%d): Test2->__clone()
#12 %s(%d): Test2->__clone()
#13 %s(%d): Test2->__clone()
#14 %s(%d): Test2->__clone()
#15 %s(%d): Test2->__clone()
#16 %s(%d): Test2->__clone()
#17 %s(%d): Test2->__clone()
#18 %s(%d): Test2->__clone()
#19 %s(%d): Test2->__clone()
#20 %s(%d): Test2->__clone()
#21 {main}
