--TEST--
Test basic function : variation4 use_trans_sid
--INI--
session.use_strict_mode=0
session.use_only_cookies=0
session.use_trans_sid=1
session.save_handler=files
session.hash_bits_per_character=4
session.hash_function=0
session.gc_probability=1
session.gc_divisor=1000
session.gc_maxlifetime=300
session.save_path=
session.name=PHPSESSID
session.trans_sid_tags="a=href,area=href,frame=src,form="
url_rewriter.tags="a=href,area=href,frame=src,form="
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php

ob_start();

/*
 * Prototype : session.use_trans_sid=1
 * Description : Test basic functionality.
 * Source code : ext/session/session.c
 */

echo "*** Testing basic session functionality : variation4 use_trans_sid ***\n";

echo "*** Test trans sid ***\n";
output_add_rewrite_var('testvar1','testvalue1');

session_id('test1');
session_start();

echo '
<a href="/">
<form action="" method="post">
</form>
';

session_commit();

output_add_rewrite_var('testvar2','testvalue2');

session_id('test2');
session_start();
echo '
<a href="/">
<form action="" method="post">
</form>
';
--EXPECT--
*** Testing basic session functionality : variation4 use_trans_sid ***
*** Test trans sid ***

<a href="/?PHPSESSID=test2&testvar1=testvalue1&testvar2=testvalue2">
<form action="" method="post"><input type="hidden" name="testvar1" value="testvalue1" /><input type="hidden" name="testvar2" value="testvalue2" /><input type="hidden" name="PHPSESSID" value="test2" />
</form>

<a href="/?PHPSESSID=test2&testvar1=testvalue1&testvar2=testvalue2">
<form action="" method="post"><input type="hidden" name="testvar1" value="testvalue1" /><input type="hidden" name="testvar2" value="testvalue2" /><input type="hidden" name="PHPSESSID" value="test2" />
</form>
