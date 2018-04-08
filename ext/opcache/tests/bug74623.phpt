--TEST--
Bug #74623: Infinite loop in type inference when using HTMLPurifier
--FILE--
<?php

function crash($arr) {
    $current_item = false;

    foreach($arr as $item) {
        if($item->name === 'string') {
            $current_item = $item;
        } else {
            $current_item->a[] = '';
        }
    }

}

?>
===DONE===
--EXPECT--
===DONE===
