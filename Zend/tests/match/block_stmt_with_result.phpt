--TEST--
Match statement block must not return a value
--FILE--
<?php
match (1) {
    1 => { new stdClass },
};
?>
===DONE===
--EXPECT--
===DONE===
