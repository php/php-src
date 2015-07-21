--TEST--
Bug #67972: SessionHandler Invalid memory read create_sid()
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php

(new SessionHandler)->create_sid();
--EXPECTF--
Fatal error: SessionHandler::create_sid(): Cannot call default session handler in %s on line %d
