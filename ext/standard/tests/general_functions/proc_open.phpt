--TEST--
proc_open
--SKIPIF--
<?php # vim:syn=php
if (!is_executable("/bin/cat")) echo "skip";
?>
--POST--
--GET--
--FILE--
<?php
$ds = array(
		0 => array("pipe", "r"),
		1 => array("pipe", "w"),
		2 => array("pipe", "w")
		);

$cat = proc_open(
		"/bin/cat",
		$ds,
		$pipes
		);

/* As per manual: avoid deadlock */
for ($i = 0; $i < count($pipes); $i++)
{
    fclose($pipes[$i]);
}

proc_close($cat);

echo "I didn't segfault!\n";

?>
--EXPECT--
I didn't segfault!
