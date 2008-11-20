--TEST--
wddx packet construction using wddx ressource
--SKIPIF--
<?php if (!extension_loaded("wddx")) print "skip"; ?>
--INI--
precision=14
--FILE--
<?php
	$pkt = wddx_packet_start('TEST comment');

	$var1 = NULL;
	$var2 = 'some string';
	$var3 = 756;
	$var4 = true;

	// add vars to packet
	wddx_add_vars($pkt, 'var1', 'var2', array('var3', 'var4'));
	echo wddx_packet_end($pkt);
?>
--EXPECT--
<wddxPacket version='1.0'><header><comment>TEST comment</comment></header><data><struct><var name='var1'><null/></var><var name='var2'><string>some string</string></var><var name='var3'><number>756</number></var><var name='var4'><boolean value='true'/></var></struct></data></wddxPacket>
