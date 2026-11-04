--TEST--
GH-24641 (ndbm backend never checks dbm_open() return value, NULL deref on first use after a failed open)
--EXTENSIONS--
dba
--FILE--
<?php

file_put_contents(__DIR__ . '/gh24641', 'not a valid ndbm/gdbm database, just plain text');

foreach (dba_handlers() as $handler) {
	/* Suppress warnings */
	$db = @dba_open(__DIR__ . '/gh24641', 'r', $handler);
	/* Expected as this file is not a proper db file */
	if ($db === false) {
		continue;
	}
	/* Some drivers just don't care */
    dba_firstkey($db); // SEGV
    dba_close($db);
}

?>
DONE
--CLEAN--
<?php
@unlink(__DIR__ . '/gh24641');
@unlink(__DIR__ . '/gh24641-lock');
?>
--EXPECT--
DONE
