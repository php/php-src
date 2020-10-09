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
try {
    jdtojewish(gregoriantojd(1,1,9998),true);
} catch (ValueError $ex) {
    echo "{$ex->getMessage()}\n";
}
?>
--EXPECTF--
string(%d) "2/22/5763
�� ���� �����
�� ���� �'����
�� ���� � ����� ����
�� ���� �' ����� ����
�"� ���� ����"�
�' ���� ����"�
�' ���� �'���"�
�' ���� � ����� ���"�
�' ���� �' ����� ���"�
7/20/5767
"
2/30/5763
3/16/5765
3/8/13758
Year out of range (0-9999)
