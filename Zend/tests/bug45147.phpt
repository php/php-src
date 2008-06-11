--TEST--
Bug #45147 (unexpected T_ENDFOR)
--XFAIL--
http://bugs.php.net/bug.php?id=45147
--FILE--
<?php for ($i = 0; $i == 0; $i++): ?>
		<?php if (true): ?>#<?php else: ?>#<?php endif; ?>
<?php endfor; ?>
--EXPECT--
                #
