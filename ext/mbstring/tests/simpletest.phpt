--TEST--
Simple multi-byte print test (EUC-JP)
--SKIPIF--
<?php extension_loaded('mbstring') or die('skip mbstring not available'); ?>
--INI--
output_handler=
--FILE--
<?php
/*
 * Test basic PHP functions to check if it works with multi-byte chars
 */

// EUC-JP strings
$s1 = "�ޥ���Х��ȴؿ����Ȥ��ޤ���";
$s2 = "����ʸ����Ϣ�뤵��Ƥ���Ϥ���";

// print directly
echo "echo: ".$s1.$s2."\n";
print("print: ".$s1.$s2."\n");
printf("printf: %s%s\n",$s1, $s2);
echo sprintf("sprintf: %s%s\n",$s1, $s2);

// Assign to var
$s3 = $s1.$s2."\n";
echo "echo: ".$s3;

?>
--EXPECT--
echo: �ޥ���Х��ȴؿ����Ȥ��ޤ�������ʸ����Ϣ�뤵��Ƥ���Ϥ���
print: �ޥ���Х��ȴؿ����Ȥ��ޤ�������ʸ����Ϣ�뤵��Ƥ���Ϥ���
printf: �ޥ���Х��ȴؿ����Ȥ��ޤ�������ʸ����Ϣ�뤵��Ƥ���Ϥ���
sprintf: �ޥ���Х��ȴؿ����Ȥ��ޤ�������ʸ����Ϣ�뤵��Ƥ���Ϥ���
echo: �ޥ���Х��ȴؿ����Ȥ��ޤ�������ʸ����Ϣ�뤵��Ƥ���Ϥ���
