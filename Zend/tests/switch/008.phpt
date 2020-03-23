--TEST--
Switch expression precedence
--FILE--
<?php

print !true switch {
    false => "! has higher precedence\n"
};

$throwableInterface = Throwable::class;
print new RuntimeException() instanceof $throwableInterface switch {
    true => "instanceof has higher precedence\n"
};

print 10 ** 2 switch {
    100 => "** has higher precedence\n"
};

?>
--EXPECT--
! has higher precedence
instanceof has higher precedence
** has higher precedence
