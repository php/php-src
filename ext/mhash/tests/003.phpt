--TEST--
mhash_keygen_s2k() test
--SKIPIF--
<?php
	include "skip.inc";
?>
--FILE--
<?php

$supported_hash_al = array(
"MHASH_MD5"       => "†\x15N¯”2Íé4z¡P”¹ÀFë\x06æ ”\x0CTyªzcg®hµààt^W\x09þÞ-Ÿésš­A7Yú§:Ìí‚\x10w´Ý²x€dãqëS³©Õ^Òƒš«&UÈ,þÛè: „aÇ™Ù×zä\x06\x1CS›\x01",
"MHASH_SHA1"      => "Ý1\\p\x06\x1D\x07E]SÂû\x0B\x08ß\x0Caªf\\\x1A±§\x01ú\x10•T#\$‹¨2¥­ä\x06³›xc\x0Aº=\x16ˆæ\"IJ\x0E®'NÎšÔ½÷n‡Ë\x08J3É\x15<+H\x13\x1D0§[\x00§À[‘ñÿê¿Y»\x12qÄØ¡\x19¸K¯mI",
"MHASH_HAVAL256"  => "ÞG\x00Ÿ‡Õé¢NÏPwÖ\x0CH6W¥Ù„«+·€õ‡,Êö\x1C\x0DgdZ„ŽUþá\x07)oAiÉ[Nað®îú²d…TÁ\x17\x1F°¢ü2ªZîÓÕÁUÓ46}IYb,ÚÞþC®\x17½\x1AuùÔþ÷{ñ’¾[x",
"MHASH_HAVAL224"  => "\\Jÿ=‚ZÖ\x08ö\x08Èêçyî8haÆ\x0A˜ó×p³¦g|yÂÚÜ«qÝàÀ\x19\x1E\x06ƒ—«)
åË¼l¼ÐÇŒ¨G\x0CB@kwèÂºQ“ù‚v52O¸P¬-0·5\x00L	m`G-2ÃI",
"MHASH_HAVAL192"  => "\"àÂq&\x02<….ùA\x07»/\x1Eá2°d\x17‹Ëû\x1C2æXv\x0Bp½Å±Å%™\x03\x16(ÂC;î+\x08p«z8®²\x12\x15\x13NÁ\x08‰u¹©d‡d)qïž³Ù‡ºùv_ÙæÖMINª„¯çà xLt—žº±Ç‡",
"MHASH_HAVAL160"  => "Öåðï\x07óúÌídní¶6GXìÞmÆû\x06\x1E\x00¤–õÎ·#÷Ž¡5ˆM–‚\"míiÁ\x1D„1\$\x0Eù|­X<O)Y;¿=ÓÊ°¸y.³Ø`\"Ê`\x02ëÐÙ´B™	Ô¯…¾Òµ©k>G¹¸ÊÉ\x19Á\x17~Ä\x0D~",
"MHASH_RIPEMD160" => "äÕÛFšòŸxâ¹\x0DÇ5ÉÏ\x02\x0a\x1D[\x19¦gDXgw”ÔÜ¡DÔ&Åb¯ùŽ†jŠ’B™ëö°êš7ù‡¡û]é¶GíÃ[\x14G`^\x1B«Ã\x08Kç “\x11\x17ë3C-ABâ%ß\x04K\x03??öK´¡†‚¤ù",
"MHASH_GOST"      => "ÀDöi½~†C•=wÆ‚ý\x17’BÙß\x15}­øs¾M–\x01äd|\x01‚4h“Yç\"\x0A°I*b@Ñ„Äxc@sÞ¨y¾†ýN%d÷×\x09¶ŠFD\x0a\x12\x12Pà\x0FÇÕ}E©À~â:pOôŒ\x0D­pwìR{\x19M‡",
"MHASH_TIGER"     => "gêÉ{Ê\nG±ö&/3däÎ#7`þ2UöBQ/Óy)ºÌñçX#k'h¤ÂÀÀn‹ä/¥÷E‚¸©›ÛÀ˜p*M1 VÄÉKÚyºO`Õ	×ø•M¨¢——6Ô|\"ª",
"MHASH_CRC32"     => "H@Œ&_šYïèoQÅÒÐ1[Fq\"®\x08ÁÌÝfŸéVÈ8\x08ã¥EöîE<¤™M¼:Èº…¥r„ðk*±HœÚu±è/}›ßÛh3iW»ñžJ?vu
i…¥=ÕW\"üó",
"MHASH_CRC32B"    => "µl«e¦>}û*©]¶F×›6\x13ŠbCÍË.	I¯\x0F–jœÎ¥0ÐÛ\x0D\x1F<˜Æ.Qyç–¾¶tiý°xbØ\$}ƒ\x0Bõ˜È´“	×Ï¬ÈŒDÅDK…\x13é1uLðÝ6§¡`÷æÉ|Ecñ\x04°",
);

foreach ($supported_hash_al as $hash=>$wanted) {
	$passwd = str_repeat($hash, 10);
	$salt = str_repeat($hash, 2);
	$result = mhash_keygen_s2k(constant($hash), $passwd, $salt, 100);
	if (!strcmp($result, $wanted)) {
		echo "$hash\nok\n";
	} else {
		echo "$hash: ";
		var_dump(bin2hex($wanted));
		echo "$hash: ";
		var_dump(bin2hex($result));
	}
	echo "\n";
}
?>
--EXPECT--
MHASH_MD5
ok

MHASH_SHA1
ok

MHASH_HAVAL256
ok

MHASH_HAVAL224
ok

MHASH_HAVAL192
ok

MHASH_HAVAL160
ok

MHASH_RIPEMD160
ok

MHASH_GOST
ok

MHASH_TIGER
ok

MHASH_CRC32
ok

MHASH_CRC32B
ok
