<?php
if (!extension_loaded("xml")) {
  $dlext = (substr(PHP_OS, 0, 3) == "WIN") ? ".dll" : ".so";
  @dl("xml$dlext");
}
if (!extension_loaded("xml")) {
  print "skip\n";
}
?>
