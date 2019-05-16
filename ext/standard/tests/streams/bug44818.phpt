--TEST--
Bug #44818 (php://memory writeable when opened read only)
--FILE--
<?php
function test($url, $mode) {
	echo "$url, $mode\n";
	$fd = fopen($url, $mode);
	var_dump($fd, fwrite($fd, "foo"));
	var_dump(fseek($fd, 0, SEEK_SET), fread($fd, 3));
	fclose($fd);
}
test("php://memory","r");
test("php://memory","r+");
test("php://temp","r");
test("php://temp","w");
?>
--EXPECTF--
php://memory, r
resource(%d) of type (stream)
int(0)
int(0)
string(0) ""
php://memory, r+
resource(%d) of type (stream)
int(3)
int(0)
string(3) "foo"
php://temp, r
resource(%d) of type (stream)
int(0)
int(0)
string(0) ""
php://temp, w
resource(%d) of type (stream)
int(3)
int(0)
string(3) "foo"
