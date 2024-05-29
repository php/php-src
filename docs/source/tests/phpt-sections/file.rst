##########
 --FILE--
##########

The most common way to provide the script of the test being tested. The script within the
``--FILE--`` section should be wrapped by an opening *and* closing PHP tag. As this allows to
execute the ``.phpt`` directly without invoking the test runner.

*********
 Example
*********

.. code:: php

   --FILE--
   <?php
   $r = 20 + 36;
   var_dump($r);
   ?>
