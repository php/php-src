--TEST--
Bug #18916 (xmlrpc_set_type() not working)
--INI--
date.timezone="America/Sao_Paulo"
--FILE--
<?php

$params = date("Ymd\TH:i:s", time());
xmlrpc_set_type($params, 'datetime');
echo xmlrpc_encode($params);

?>
--EXPECTF--
<?xml version="1.0" encoding="utf-8"?>
<params>
<param>
 <value>
  <dateTime.iso8601>%dT%d:%d:%d</dateTime.iso8601>
 </value>
</param>
</params>
