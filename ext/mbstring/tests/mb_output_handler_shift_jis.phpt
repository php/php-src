--TEST--
mb_output_handler() (Shift_JIS)
--SKIPIF--
<?php extension_loaded('mbstring') or die('skip mbstring not available'); ?>
--INI--
output_handler=mb_output_handler
internal_encoding=Shift_JIS
output_encoding=EUC-JP
--FILE--
<?php
// Shift_JIS
var_dump("�e�X�g�p���{�ꕶ����B���̃��W���[����PHP�Ƀ}���`�o�C�g�֐���񋟂��܂��B");
?>
--EXPECT--
string(73) "�ƥ��������ܸ�ʸ���󡣤��Υ⥸�塼���PHP�˥ޥ���Х��ȴؿ����󶡤��ޤ���"
