--TEST--
Bug #32588 (strtotime() error for 'last xxx' DST problem)
--INI--
date.timezone=America/New_York
--FILE--
<?php

echo date('D Y/m/d/H:i:s', strtotime('last saturday', 1112703348)). "\n";
echo date('D Y/m/d/H:i:s', strtotime("last sunday", 1112703348)). "\n";
echo date('D Y/m/d/H:i:s', strtotime('last monday', 1112703348)). "\n";
--EXPECT--
Sat 2005/04/02/00:00:00
Sun 2005/04/03/00:00:00
Mon 2005/04/04/00:00:00
