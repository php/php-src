--TEST--
Bug #55445 (Lexer error with short open tags)
--FILE--
<?php $u = "chris"; ?><p>Welcome <?= $u ?></p>
--EXPECT--
<p>Welcome chris</p>
