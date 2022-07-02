--TEST--
SPL: SplFileObject::setCsvControl error 002
--CREDITS--
Erwin Poeze <erwin.poeze at gmail.com>
--FILE--
<?php
file_put_contents('csv_control_data_error002.csv',
<<<CDATA
'groene appelen'|10
'gele bananen'|20
'rode kersen'|30
CDATA
);
$s = new SplFileObject('csv_control_data_error002.csv');
$s->setFlags(SplFileObject::READ_CSV);
try {
    $s->setCsvControl('|', 'two');
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}
?>
--CLEAN--
<?php
unlink('csv_control_data_error002.csv');
?>
--EXPECT--
SplFileObject::setCsvControl(): Argument #2 ($enclosure) must be a single character
