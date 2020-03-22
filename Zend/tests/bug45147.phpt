--TEST--
Bug #45147 (unexpected T_ENDFOR)
--FILE--
<?php for ($i = 0; $i == 0; $i++): ?>
        <?php if (true): ?>#<?php else: ?>#<?php endif; ?>
<?php endfor; ?>
--EXPECT--
                #
