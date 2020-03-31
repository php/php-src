--TEST--
Switch expression precedence
--FILE--
<?php

print switch (!true) {
    false => "! has higher precedence\n"
};

$throwableInterface = Throwable::class;
print switch (new RuntimeException() instanceof $throwableInterface) {
    true => "instanceof has higher precedence\n"
};

print switch (10 ** 2) {
    100 => "** has higher precedence\n"
};

?>
--EXPECT--
! has higher precedence
instanceof has higher precedence
** has higher precedence
