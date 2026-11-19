--TEST--
Bug #55399 (parse_url() incorrectly treats ':' as a valid path)
--FILE--
<?php

var_dump(parse_url(":"));

?>
--EXPECTF--
Warning: parse_url(): Trailing colon without port in URL in %s on line %d
bool(false)
