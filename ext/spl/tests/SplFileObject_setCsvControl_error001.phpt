--TEST--
SPL: SplFileObject::setCsvControl error 001
--CREDITS--
Erwin Poeze <erwin.poeze at gmail.com>
--FILE--
<?php
file_put_contents('csv_control_data_error001.csv',
<<<CDATA
'groene appelen'|10
'gele bananen'|20
'rode kersen'|30
CDATA
);
$s = new SplFileObject('csv_control_data_error001.csv');
$s->setFlags(SplFileObject::READ_CSV);

try {
    $s->setCsvControl('||');
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
?>
--CLEAN--
<?php
unlink('csv_control_data_error001.csv');
?>
--EXPECT--
delimiter must be a character
