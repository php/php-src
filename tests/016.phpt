--TEST--
Check for Yaf_Session
--SKIPIF--
<?php if (!extension_loaded("yaf")) print "skip"; ?>
--FILE--
<?php 
$session = Yaf_Session::getInstance();

$_SESSION["name"] = "Laruence";

$age = 28;
$session->age = $age;
unset($age);

unset($session);
$session2 = Yaf_Session::getInstance();
$session2["company"] = "Baidu";

var_dump(isset($session2->age));
var_dump($session2->has("name"));
var_dump(count($session2));
foreach ($session2 as $key => $value) {
	echo $key , "=>", $value, "\n";
}

unset($session2);
$session3 = Yaf_Session::getInstance();

$session3->del("name");
unset($session3["company"]);
unset($session3->age);

var_dump(count($session3));
?>
--EXPECTF--
bool(true)
bool(true)
int(3)
name=>Laruence
age=>28
company=>Baidu
int(0)
