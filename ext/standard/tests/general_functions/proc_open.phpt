--TEST--
proc_open
--SKIPIF--
<?php
if (!is_executable("/bin/cat")) echo "skip";
if (!function_exists("proc_open")) echo "skip proc_open() is not available";
?>
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

proc_close($cat);

echo "I didn't segfault!\n";

?>
--EXPECT--
I didn't segfault!
