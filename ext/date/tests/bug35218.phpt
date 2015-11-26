--TEST--
Bug #35218 (strtotime no longer ignores timezone comments like "(PST)")
--FILE--
<?php
date_default_timezone_set("UTC");
$date = 'Sun, 13 Nov 2005 22:56:10 -0800 (PST)';
$date_fixed = 'Sun, 13 Nov 2005 22:56:10 -0800';

var_dump(strtotime($date));
var_dump(strtotime($date_fixed));
?>
--EXPECT--
int(1131951370)
int(1131951370)
