--TEST--
Bug #38698 (Bug #38698 for some keys cdbmake creates corrupted db and cdb can't read valid db)
--SKIPIF--
<?php
	$handler = 'cdb_make';
	require_once(__DIR__ .'/skipif.inc');
?>
--FILE--
<?php

function isLittleEndian() {
    return 0x00FF === current(unpack('v', pack('S',0x00FF)));
}

$db_file = __DIR__ .'/129php.cdb';

if (($db_make=dba_open($db_file, "n", 'cdb_make'))!==FALSE) {
	if (isLittleEndian() === FALSE) {
        dba_insert(pack('V',129), "Booo!", $db_make);
	} else{
		dba_insert(pack('i',129), "Booo!", $db_make);
	}
	dba_close($db_make);
	// write md5 checksum of generated database file
	var_dump(md5_file($db_file));
	@unlink($db_file);
} else {
    echo "Error creating database\n";
}
?>
===DONE===
--EXPECT--
string(32) "1f34b74bde3744265acfc21e0f30af95"
===DONE===
