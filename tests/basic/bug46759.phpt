--TEST--
Testing magic_quotes_gpc
--INI--
display_errors=0
magic_quotes_gpc=1
--GET--
a='&b="&c=\"
--FILE--
<?php 

foreach ($_GET AS $key => $value)
{
	echo $key . ": " . $value . "\n";
}

?>
--EXPECT--
a: \'
b: \"
c: \\\"
