--TEST--
rewriter handles <form> and <fieldset> correctly
--SKIPIF--
<?php include('skipif.inc'); ?>
--INI--
session.use_cookies=0
session.cache_limiter=
session.use_trans_sid=1
url_rewriter.tags="a=href,area=href,frame=src,input=src,form=,fieldset="
session.name=PHPSESSID
--FILE--
<?php

error_reporting(E_ALL);

session_id("abtest");
session_start();
?>
<form>
<fieldset>
<?php

ob_flush();

ini_set("url_rewriter.tags", "a=href,area=href,frame=src,input=src,form=");

?>
<form>
<fieldset>
<?php

ob_flush();

ini_set("url_rewriter.tags", "a=href,area=href,frame=src,input=src,form=fakeentry");

?>
<form>
<fieldset>
<?php

ob_flush();

ini_set("url_rewriter.tags", "a=href,fieldset=,area=href,frame=src,input=src");

?>
<form>
<fieldset>
<?php

session_destroy();
?>
--EXPECT--
<form><input type="hidden" name="PHPSESSID" value="abtest" />
<fieldset><input type="hidden" name="PHPSESSID" value="abtest" />
<form><input type="hidden" name="PHPSESSID" value="abtest" />
<fieldset>
<form><input type="hidden" name="PHPSESSID" value="abtest" />
<fieldset>
<form>
<fieldset><input type="hidden" name="PHPSESSID" value="abtest" />
