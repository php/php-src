--TEST--
sqlite-spl: Exception
--SKIPIF--
<?php # vim:ft=php
if (!extension_loaded("sqlite")) print "skip"; 
if (!extension_loaded("spl")) print "skip SPL is not present"; 
?>
--FILE--
<?php

try
{
	$db = sqlite_factory();
}
catch(SQLiteException $e)
{
	$parents = class_parents($e);
	if (array_key_exists('RuntimeException', $parents))
	{
		echo "GOOD\n";
	}
}

?>
===DONE===
--EXPECT--
GOOD
===DONE===
