--TEST--
May escape match statement block with goto
--FILE--
<?php
match (1) {
    1 => {
        echo "Before goto\n";
        goto after;
        echo "After goto\n";
    },
};
after:
echo "After match\n";
?>
--EXPECT--
Before goto
After match
