--TEST--
filter_data() and FS_STRIPPED
--FILE--
<?php

var_dump(filter_data("<p>Let me <font color=\"#000000\">see</font> you <br /><b>Stripped</b> down to the bone</p>", FS_STRIPPED));
var_dump(filter_data("!@#$%^&*()><<>+_\"'<br><p /><li />", FS_STRIPPED));
var_dump(filter_data("", FS_STRIPPED));

var_dump(filter_data("<p>Let me <font color=\"#000000\">see</font> you <br /><b>Stripped</b> down to the bone</p>", FS_STRIPPED, FILTER_FLAG_STRIP_LOW));
var_dump(filter_data("!@#$%^&*()><<>+_\"'<br><p /><li />", FS_STRIPPED, FILTER_FLAG_STRIP_LOW));
var_dump(filter_data("", FS_STRIPPED, FILTER_FLAG_STRIP_LOW));

var_dump(filter_data("<p>Let me <font color=\"#000000\">see</font> you <br /><b>Stripped</b> down to the bone</p>", FS_STRIPPED, FILTER_FLAG_STRIP_HIGH));
var_dump(filter_data("!@#$%^&*()><<>+_\"'<br><p /><li />", FS_STRIPPED, FILTER_FLAG_STRIP_HIGH));
var_dump(filter_data("", FS_STRIPPED, FILTER_FLAG_STRIP_HIGH));

echo "Done\n";
?>
--EXPECTF--	
string(40) "Let me see you Stripped down to the bone"
string(11) "!@#$%^&*()>"
NULL
string(40) "Let me see you Stripped down to the bone"
string(11) "!@#$%^&*()>"
NULL
string(40) "Let me see you Stripped down to the bone"
string(11) "!@#$%^&*()>"
NULL
Done
