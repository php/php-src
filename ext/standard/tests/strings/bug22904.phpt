--TEST--
Bug #22904 (magic mode failed for cybase with '\0')
--FILE--
not active yet
<?php

/*
ini_set("magic_quotes_sybase","on");
test();
ini_set("magic_quotes_sybase","off");
test();
*/
function test(){
	$buf = 'g\g"\0g'."'";
	$slashed = addslashes($buf);
	echo "$buf\n";
	echo "$slashed\n";
	echo stripslashes($slashed."\n");
/*
g\g"\0g'
g\\g"\\0g''
g\g"\0g'
g\g"\0g'
g\\g\"\\0g\'
g\g"\0g'
*/
}
?>
--EXPECT--
not active yet
