<html>
<body bgcolor="white">
<head>
<title>Request Header Example</title>
</head>
<body>
<h3>Request Header Example</h3>
<table border=0>

<?php

   $e = $request->headerNames;
   while ($e->hasMoreElements()) {
     $name = $e->nextElement();
     $value = $request->getHeader($name);
     print "<tr><td bgcolor=\"#CCCCCC\">$name\n";
     print "</td><td>$value</td></tr>\n";
    }

?>

</table>
</body>
</html>
