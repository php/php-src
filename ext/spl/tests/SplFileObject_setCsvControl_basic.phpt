--TEST--
SPL: SplFileObject::setCsvControl basic
--CREDITS--
Erwin Poeze <erwin.poeze at gmail.com>
--FILE--
<?php
file_put_contents('csv_control_data_basic.csv',
<<<CDATA
'groene appelen'|10
'gele bananen'|20
'rode kersen'|30
CDATA
);
$s = new SplFileObject('csv_control_data_basic.csv');
$s->setFlags(SplFileObject::READ_CSV);
$s->setCsvControl('|', '\'', '/');
foreach ($s as $row) {
    list($fruit, $quantity) = $row;
    echo "$fruit : $quantity\n";
}
?>
--CLEAN--
<?php
unlink('csv_control_data_basic.csv');
?>
--EXPECT--
groene appelen : 10
gele bananen : 20
rode kersen : 30
