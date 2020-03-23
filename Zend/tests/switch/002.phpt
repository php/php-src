--TEST--
Switch expression omit trailing comma
--FILE--
<?php

function print_bool($bool) {
    echo $bool switch {
        true => "true\n",
        false => "false\n"
    };
}

print_bool(true);
print_bool(false);

?>
--EXPECT--
true
false
