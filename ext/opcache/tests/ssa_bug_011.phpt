--TEST--
Wrong assertion
--EXTENSIONS--
opcache
--FILE--
<?php
function foo($transitions) {
    foreach ($transitions as $transition) {
        if (isEmpty()) {
            continue;
        }
    }
}
?>
OK
--EXPECT--
OK
