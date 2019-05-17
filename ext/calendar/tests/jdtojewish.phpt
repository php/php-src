--TEST--
jdtojewish() function
--SKIPIF--
<?php include 'skipif.inc'; ?>
--FILE--
<?php

var_dump(jdtojewish(gregoriantojd(10,28,2002))."\r\n".
	jdtojewish(gregoriantojd(10,28,2002),true)."\r\n".
	jdtojewish(gregoriantojd(10,28,2002),true, CAL_JEWISH_ADD_ALAFIM_GERESH)."\r\n".
	jdtojewish(gregoriantojd(10,28,2002),true, CAL_JEWISH_ADD_ALAFIM)."\r\n".
	jdtojewish(gregoriantojd(10,28,2002),true, CAL_JEWISH_ADD_ALAFIM_GERESH+CAL_JEWISH_ADD_ALAFIM)."\r\n".
	jdtojewish(gregoriantojd(10,28,2002),true, CAL_JEWISH_ADD_GERESHAYIM)."\r\n".
	jdtojewish(gregoriantojd(10,8,2002),true, CAL_JEWISH_ADD_GERESHAYIM)."\r\n".
	jdtojewish(gregoriantojd(10,8,2002),true, CAL_JEWISH_ADD_GERESHAYIM+CAL_JEWISH_ADD_ALAFIM_GERESH)."\r\n".
	jdtojewish(gregoriantojd(10,8,2002),true, CAL_JEWISH_ADD_GERESHAYIM+CAL_JEWISH_ADD_ALAFIM)."\r\n".
	jdtojewish(gregoriantojd(10,8,2002),true, CAL_JEWISH_ADD_GERESHAYIM+CAL_JEWISH_ADD_ALAFIM+CAL_JEWISH_ADD_ALAFIM_GERESH)."\r\n".
	jdtojewish(gregoriantojd(3,10,2007))."\r\n");

echo jdtojewish(gregoriantojd(11,5,2002)) . "\n";
echo jdtojewish(gregoriantojd(11,29,2004)) . "\n";
echo jdtojewish(gregoriantojd(1,1,9998)) . "\n";
echo jdtojewish(gregoriantojd(1,1,9998),true) . "\n";
?>
--EXPECTF--
string(%d) "2/22/5763
כב חשון התשסג
כב חשון ה'תשסג
כב חשון ה אלפים תשסג
כב חשון ה' אלפים תשסג
כ"ב חשון התשס"ג
ב' חשון התשס"ג
ב' חשון ה'תשס"ג
ב' חשון ה אלפים תשס"ג
ב' חשון ה' אלפים תשס"ג
7/20/5767
"
2/30/5763
3/16/5765
3/8/13758

Warning: jdtojewish(): Year out of range (0-9999) in %s on line %d
