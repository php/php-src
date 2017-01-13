--TEST--
Bug #70661 (Use After Free Vulnerability in WDDX Packet Deserialization)
--SKIPIF--
<?php
if (!extension_loaded("wddx")) print "skip";
?>
--FILE--
<?php
$fakezval = ptr2str(1122334455);
$fakezval .= ptr2str(0);
$fakezval .= "\x00\x00\x00\x00";
$fakezval .= "\x01";
$fakezval .= "\x00";
$fakezval .= "\x00\x00";

$x = <<<EOT
<?xml version='1.0'?>
<wddxPacket version='1.0'>
<header/>
	<data>
		<struct>
			<recordset rowCount='1' fieldNames='ryat'>
				<field name='ryat'>
					<var name='php_class_name'>
						<string>stdClass</string>
					</var>
					<null/>
				</field>
			</recordset>
		</struct>	
	</data>
</wddxPacket>
EOT;

$y = wddx_deserialize($x);

for ($i = 0; $i < 5; $i++) {
	$v[$i] = $fakezval.$i;
}

var_dump($y);

function ptr2str($ptr)
{
	$out = '';
	
	for ($i = 0; $i < 8; $i++) {
		$out .= chr($ptr & 0xff);
		$ptr >>= 8;
	}
	
	return $out;
}
?>
DONE
--EXPECTF--
array(1) {
  [0]=>
  array(1) {
    ["ryat"]=>
    array(2) {
      ["php_class_name"]=>
      string(8) "stdClass"
      [0]=>
      NULL
    }
  }
}
DONE