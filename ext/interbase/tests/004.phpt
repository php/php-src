--TEST--
InterBase: BLOB test
--SKIPIF--
<?php include("skipif.inc"); ?>
--FILE--
<?php

    require("interbase.inc");

    $link = ibase_connect($test_base);

    ibase_query(
    	"CREATE TABLE test4 (
    		v_integer   integer,
            v_blob		blob)");
    ibase_commit();

    /* create 100k blob file  */
    $blob_str = rand_binstr(100*1024);

    $name = tempnam(dirname(__FILE__),"blob.tmp");
    $ftmp = fopen($name,"w");
    fwrite($ftmp,$blob_str);
    fclose($ftmp);

    echo "import blob 1\n";
    $ftmp = fopen($name,"r");
    $bl_s = ibase_blob_import($ftmp);
    ibase_query("INSERT INTO test4 (v_integer, v_blob) VALUES (1, ?)", $bl_s);

    $bl_s = ibase_blob_import($link,$ftmp);
    ibase_query($link, "INSERT INTO test4 (v_integer, v_blob) VALUES (1, ?)", $bl_s);
    fclose($ftmp);

    echo "test blob 1\n";
    $q = ibase_query("SELECT v_blob FROM test4 WHERE v_integer = 1");

    $row = ibase_fetch_object($q);
    $bl_h = ibase_blob_open($row->V_BLOB);

	$blob = '';
    while($piece = ibase_blob_get($bl_h, 1 + rand() % 1024))
        $blob .= $piece;
    if($blob != $blob_str)
		echo " BLOB 1 fail (1)\n";
    ibase_blob_close($bl_h);

    $bl_h = ibase_blob_open($link,$row->V_BLOB);

	$blob = '';
    while($piece = ibase_blob_get($bl_h, 100 * 1024))
        $blob .= $piece;
    if($blob != $blob_str)
		echo " BLOB 1 fail (2)\n";
    ibase_blob_close($bl_h);
    ibase_free_result($q);
    unset($blob);

    echo "create blob 2\n";

    ibase_query("INSERT INTO test4 (v_integer, v_blob) VALUES (2, ?)", $blob_str);

    echo "test blob 2\n";

    $q = ibase_query("SELECT v_blob FROM test4 WHERE v_integer = 2");
    $row = ibase_fetch_object($q,IBASE_TEXT);

    if($row->V_BLOB != $blob_str)
		echo " BLOB 2 fail\n";
    ibase_free_result($q);
    unset($blob);


    echo "create blob 3\n";

    $bl_h = ibase_blob_create($link);

    ibase_blob_add($bl_h, "+----------------------------------------------------------------------+\n");
    ibase_blob_add($bl_h, "| PHP HTML Embedded Scripting Language Version 3.0                     |\n");
    ibase_blob_add($bl_h, "+----------------------------------------------------------------------+\n");
    ibase_blob_add($bl_h, "| Copyright (c) 1997-2000 PHP Development Team (See Credits file)      |\n");
    ibase_blob_add($bl_h, "+----------------------------------------------------------------------+\n");
    ibase_blob_add($bl_h, "| This program is free software; you can redistribute it and/or modify |\n");
    ibase_blob_add($bl_h, "| it under the terms of one of the following licenses:                 |\n");
    ibase_blob_add($bl_h, "|                                                                      |\n");
    ibase_blob_add($bl_h, "|  A) the GNU General Public License as published by the Free Software |\n");
    ibase_blob_add($bl_h, "|     Foundation; either version 2 of the License, or (at your option) |\n");
    ibase_blob_add($bl_h, "|     any later version.                                               |\n");
    ibase_blob_add($bl_h, "|                                                                      |\n");
    ibase_blob_add($bl_h, "|  B) the PHP License as published by the PHP Development Team and     |\n");
    ibase_blob_add($bl_h, "|     included in the distribution in the file: LICENSE                |\n");
    ibase_blob_add($bl_h, "|                                                                      |\n");
    ibase_blob_add($bl_h, "| This program is distributed in the hope that it will be useful,      |\n");
    ibase_blob_add($bl_h, "| but WITHOUT ANY WARRANTY; without even the implied warranty of       |\n");
    ibase_blob_add($bl_h, "| MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        |\n");
    ibase_blob_add($bl_h, "| GNU General Public License for more details.                         |\n");
    ibase_blob_add($bl_h, "|                                                                      |\n");
    ibase_blob_add($bl_h, "| You should have received a copy of both licenses referred to here.   |\n");
    ibase_blob_add($bl_h, "| If you did not, or have any questions about PHP licensing, please    |\n");
    ibase_blob_add($bl_h, "| contact core@php.net.                                                |\n");
    ibase_blob_add($bl_h, "+----------------------------------------------------------------------+\n");
    $bl_s = ibase_blob_close($bl_h);
    ibase_query("INSERT INTO test4 (v_integer, v_blob) VALUES (3, ?)", $bl_s);
	ibase_commit();
    echo "echo blob 3\n";

    $q = ibase_query("SELECT v_blob FROM test4 WHERE v_integer = 3");
    $row = ibase_fetch_object($q);
	ibase_commit();
	ibase_close();

    $link = ibase_connect($test_base);
    ibase_blob_echo($link, $row->V_BLOB);
    ibase_free_result($q);

    echo "fetch blob 3\n";
    $q = ibase_query("SELECT v_blob FROM test4 WHERE v_integer = 3");
    $row = ibase_fetch_object($q,IBASE_TEXT);
    echo $row->V_BLOB;
    ibase_free_result($q);

    ibase_close();
    unlink($name);
    echo "end of test\n";
?>
--EXPECT--
import blob 1
test blob 1
create blob 2
test blob 2
create blob 3
echo blob 3
+----------------------------------------------------------------------+
| PHP HTML Embedded Scripting Language Version 3.0                     |
+----------------------------------------------------------------------+
| Copyright (c) 1997-2000 PHP Development Team (See Credits file)      |
+----------------------------------------------------------------------+
| This program is free software; you can redistribute it and/or modify |
| it under the terms of one of the following licenses:                 |
|                                                                      |
|  A) the GNU General Public License as published by the Free Software |
|     Foundation; either version 2 of the License, or (at your option) |
|     any later version.                                               |
|                                                                      |
|  B) the PHP License as published by the PHP Development Team and     |
|     included in the distribution in the file: LICENSE                |
|                                                                      |
| This program is distributed in the hope that it will be useful,      |
| but WITHOUT ANY WARRANTY; without even the implied warranty of       |
| MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        |
| GNU General Public License for more details.                         |
|                                                                      |
| You should have received a copy of both licenses referred to here.   |
| If you did not, or have any questions about PHP licensing, please    |
| contact core@php.net.                                                |
+----------------------------------------------------------------------+
fetch blob 3
+----------------------------------------------------------------------+
| PHP HTML Embedded Scripting Language Version 3.0                     |
+----------------------------------------------------------------------+
| Copyright (c) 1997-2000 PHP Development Team (See Credits file)      |
+----------------------------------------------------------------------+
| This program is free software; you can redistribute it and/or modify |
| it under the terms of one of the following licenses:                 |
|                                                                      |
|  A) the GNU General Public License as published by the Free Software |
|     Foundation; either version 2 of the License, or (at your option) |
|     any later version.                                               |
|                                                                      |
|  B) the PHP License as published by the PHP Development Team and     |
|     included in the distribution in the file: LICENSE                |
|                                                                      |
| This program is distributed in the hope that it will be useful,      |
| but WITHOUT ANY WARRANTY; without even the implied warranty of       |
| MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        |
| GNU General Public License for more details.                         |
|                                                                      |
| You should have received a copy of both licenses referred to here.   |
| If you did not, or have any questions about PHP licensing, please    |
| contact core@php.net.                                                |
+----------------------------------------------------------------------+
end of test
