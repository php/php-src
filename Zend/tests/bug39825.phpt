--TEST--
Bug #39825 (foreach produces memory error)
--FILE--
<?php 
$array = array(1 => 2, "foo" => "bar");
$obj = (object)$array;
foreach ($obj as $name => $value)  {
	echo "$name -> $value\n";
}
?>
--EXPECT--
1 -> 2
foo -> bar
