<?php
$a = true;
if ($a) {
    function foo(...$arg) {
        echo "Hello\n";
    }
}