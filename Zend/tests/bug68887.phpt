--TEST--
Bug #68887 (resources are not freed correctly)
--FILE--
<?php
fclose(fopen("php://temp","w+"));
$count = count(get_resources());
fclose(fopen("php://temp","w+"));
var_dump(count(get_resources()) == $count);
fclose(fopen("php://temp","w+"));
var_dump(count(get_resources()) == $count);
?>
--EXPECT--
bool(true)
bool(true)
