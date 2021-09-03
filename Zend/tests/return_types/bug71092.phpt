--TEST--
Bug #71092 (Segmentation fault with return type hinting)
--FILE--
<?php

function boom(): array {
    $data = [['id']];
    switch ($data[0]) {
    case ['id']:
        return null;
    }
}

boom();
?>
--EXPECTF--
Fatal error: Uncaught TypeError: boom(): Return value must be of type array, null returned in %s:%d
Stack trace:
#0 %s(%d): boom()
#1 {main}
  thrown in %sbug71092.php on line %d
