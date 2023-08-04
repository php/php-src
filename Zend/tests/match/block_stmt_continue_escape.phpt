--TEST--
Match statement block may continue out of block, with a warning
--FILE--
<?php
match (1) {
    1 => {
        echo "Before continue\n";
        continue;
        echo "After continue\n";
    },
};
echo "After match\n";
?>
--EXPECTF--
Warning: "continue" targeting switch is equivalent to "break" in %s on line %d
Before continue
After match
