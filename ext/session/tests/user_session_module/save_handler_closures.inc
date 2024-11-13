<?php

require_once 'save_handler.inc';

foreach (array ('open', 'close', 'read', 'write', 'destroy', 'gc') as $fn) {
    ${$fn.'_closure'} = function () use ($fn) { return call_user_func_array ($fn, func_get_args ()); };
}

?>
