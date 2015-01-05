--TEST--
SPL: SplFileObject::setCsvControl variation 001
--CREDITS--
Erwin Poeze <erwin.poeze at gmail.com>
--FILE--
<?php
file_put_contents('csv_control_data.csv',
<<<CDATA
"groene appelen",10
"gele bananen",20
"rode kersen",30
CDATA
);
$s = new SplFileObject('csv_control_data.csv');
$s->setFlags(SplFileObject::READ_CSV);
$s->setCsvControl();
foreach ($s as $row) {
    list($fruit, $quantity) = $row;
    echo "$fruit : $quantity\n";
}
?>
--CLEAN--
<?php
unlink('csv_control_data.csv');
?>
--EXPECT--
groene appelen : 10
gele bananen : 20
rode kersen : 30

