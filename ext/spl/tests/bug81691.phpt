--TEST--
Bug #81691 fclose on SplFileObject stream
--FILE--
<?php
$obj = new SplFileObject(__FILE__);
$resources = get_resources();
fclose(end($resources));

var_dump($obj->fgets());

?>
--EXPECTF--
Warning: fclose(): cannot close the provided stream, as it must not be manually closed in %s on line %d
string(6) "<?php
"
