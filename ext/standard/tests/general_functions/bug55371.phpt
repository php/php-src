--TEST--
Bug #55371 (get_magic_quotes_gpc() and get_magic_quotes_runtime() throw deprecated warning)
--FILE--
<?php

get_magic_quotes_gpc();
get_magic_quotes_runtime();

echo 'done';

?>
--EXPECTF--
Deprecated: Function get_magic_quotes_gpc() is deprecated in %s on line %d

Deprecated: Function get_magic_quotes_runtime() is deprecated in %s on line %d
done
