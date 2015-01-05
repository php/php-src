--TEST--
Trying declare interface with repeated name of inherited method
--FILE--
<?php 

interface a {
	function b();
}

interface b {
	function b();
}

interface c extends a, b {
}

echo "done!\n";

?>
--EXPECTF--
done!
