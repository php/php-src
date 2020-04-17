--TEST--
Test basic basic block expression
--FILE--
<?php

echo {
    echo "a\n";
    echo "b\n";
    "c\n"
};

echo { "d\n" };

echo 'e' . { "f\n" };

echo { 'g' } . "h\n";

echo {
    echo "i\n";
    { "j\n" }
};

?>
--EXPECT--
a
b
c
d
ef
gh
i
j
