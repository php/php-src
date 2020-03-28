--TEST--
Test basic basic block expression
--FILE--
<?php

print {
    echo "a\n";
    echo "b\n";
    "c\n"
};

print { "d\n" };

print 'e' . { 'f' };

?>
--EXPECT--
a
b
c
d
ef
