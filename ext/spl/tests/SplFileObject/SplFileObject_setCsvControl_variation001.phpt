--TEST--
SPL: SplFileObject::setCsvControl variation 001
--CREDITS--
Erwin Poeze <erwin.poeze at gmail.com>
--FILE--
<?php
file_put_contents('csv_control_data_variation001.csv',
<<<CDATA
"groene appelen",10
"gele bananen",20
"rode kersen",30
CDATA
);
$s = new SplFileObject('csv_control_data_variation001.csv');
$s->setFlags(SplFileObject::READ_CSV);
$s->setCsvControl();
foreach ($s as $row) {
    list($fruit, $quantity) = $row;
    echo "$fruit : $quantity\n";
}
?>
--CLEAN--
<?php
unlink('csv_control_data_variation001.csv');
?>
--EXPECTF--
Deprecated: SplFileObject::setCsvControl(): the $escape parameter must be provided as its default value will change in %s on line %d
groene appelen : 10
gele bananen : 20
rode kersen : 30
