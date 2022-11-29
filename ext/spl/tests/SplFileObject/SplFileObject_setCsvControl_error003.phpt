--TEST--
SPL: SplFileObject::setCsvControl error 003
--CREDITS--
Erwin Poeze <erwin.poeze at gmail.com>
--INI--
include_path=.
--FILE--
<?php
file_put_contents('csv_control_data_error003.csv',
<<<CDATA
'groene appelen'|10
'gele bananen'|20
'rode kersen'|30
CDATA
);
$s = new SplFileObject('csv_control_data_error003.csv');
$s->setFlags(SplFileObject::READ_CSV);
try {
    $s->setCsvControl('|', '\'', 'three');
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}
?>
--CLEAN--
<?php
unlink('csv_control_data_error003.csv');
?>
--EXPECT--
SplFileObject::setCsvControl(): Argument #3 ($escape) must be empty or a single character
