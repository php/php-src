--TEST--
jdtojewish() function
--FILE--
<?

var_dump(jdtojewish(gregoriantojd(10,28,2002))."\r\n".
	jdtojewish(gregoriantojd(10,28,2002),true)."\r\n".
	jdtojewish(gregoriantojd(10,28,2002),true, CAL_JEWISH_ADD_ALAFIM_GERESH)."\r\n".
	jdtojewish(gregoriantojd(10,28,2002),true, CAL_JEWISH_ADD_ALAFIM)."\r\n".
	jdtojewish(gregoriantojd(10,28,2002),true, CAL_JEWISH_ADD_ALAFIM_GERESH+CAL_JEWISH_ADD_ALAFIM)."\r\n".
	jdtojewish(gregoriantojd(10,28,2002),true, CAL_JEWISH_ADD_GERESHAYIM)."\r\n".
	jdtojewish(gregoriantojd(10,8,2002),true, CAL_JEWISH_ADD_GERESHAYIM)."\r\n".
	jdtojewish(gregoriantojd(10,8,2002),true, CAL_JEWISH_ADD_GERESHAYIM+CAL_JEWISH_ADD_ALAFIM_GERESH)."\r\n".
	jdtojewish(gregoriantojd(10,8,2002),true, CAL_JEWISH_ADD_GERESHAYIM+CAL_JEWISH_ADD_ALAFIM)."\r\n".
	jdtojewish(gregoriantojd(10,8,2002),true, CAL_JEWISH_ADD_GERESHAYIM+CAL_JEWISH_ADD_ALAFIM+CAL_JEWISH_ADD_ALAFIM_GERESH)."\r\n");
?>
--EXPECT--
string(184) "2/22/5763
כב חשון התשסג
כב חשון ה'תשסג
כב חשון ה אלפים תשסג
כב חשון ה' אלפים תשסג
כ"ב חשון התשס"ג
ב' חשון התשס"ג
ב' חשון ה'תשס"ג
ב' חשון ה אלפים תשס"ג
ב' חשון ה' אלפים תשס"ג
"
