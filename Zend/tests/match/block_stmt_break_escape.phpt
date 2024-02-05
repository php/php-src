--TEST--
Match statement block may break out of block
--FILE--
<?php
match (1) {
    1 => {
        echo "Before break\n";
        break;
        echo "After break\n";
    },
};
echo "After match\n";
?>
--EXPECT--
Before break
After match
