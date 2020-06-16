--TEST--
Bug #21453 (handling of non-encoded <)
--FILE--
<?php
$test = "
<table>
    <tr><td>first cell before < first cell after</td></tr>
    <tr><td>second cell before < second cell after</td></tr>
</table>";

var_dump(strip_tags($test));
?>
--EXPECTF--
string(8%d) "

    first cell before < first cell after
    second cell before < second cell after
"
