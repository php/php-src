<?php

function set_sql($name,  $query)
{
    if (empty($GLOBALS['SQL'][$name]))
    {
        $GLOBALS['SQL'][$name] = $query;
    }
}
?>
