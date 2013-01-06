--TEST--
Bug #55445 (Lexer error with short open tags)
--INI--
short_open_tag=0
--FILE--
<?php $u = "chris"; ?><p>Welcome <?= $u ?></p>
--EXPECTF--
<p>Welcome chris</p>
