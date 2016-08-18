--TEST--
Bug #69976 (Unable to parse "all" urls with colon char)
--FILE--
<?php
var_dump(parse_url("/busca/?fq=B:20001"));
var_dump(parse_url("/busca/?fq=B:200013"));
var_dump(parse_url("/busca/?fq=home:01234"));
var_dump(parse_url("/busca/?fq=home:012345"));
?>
--EXPECT--
array(2) {
  ["path"]=>
  string(7) "/busca/"
  ["query"]=>
  string(10) "fq=B:20001"
}
array(2) {
  ["path"]=>
  string(7) "/busca/"
  ["query"]=>
  string(11) "fq=B:200013"
}
array(2) {
  ["path"]=>
  string(7) "/busca/"
  ["query"]=>
  string(13) "fq=home:01234"
}
array(2) {
  ["path"]=>
  string(7) "/busca/"
  ["query"]=>
  string(14) "fq=home:012345"
}
