--TEST--
Incorrect CFG/SSA construction for SWITCH with few identical successors
--EXTENSIONS--
opcache
--FILE--
<?php
function render($properties) {
    foreach ($properties as $key => $value) {
        switch ($key) {
            case 'Trapped':
                if ($value == null) {
                    $docInfo->$key = 1;
                }
            case 'CreationDate':
            case 'ModDate':
                $docInfo->$key = 2;
                break;
        }
    }
}
?>
OK
--EXPECT--
OK
