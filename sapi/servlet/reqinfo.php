<html>
<body>
<head>
<title>Request Information Example</title>
</head>
<body bgcolor="white">
<h3>Request Information Example</h3>
<table border=0><tr><td>
Method:
</td><td>
<?php print $request->method ?>
</td></tr><tr><td>
Request URI:
</td><td>
<?php print $request->requestURI ?>
</td></tr><tr><td>
Protocol:
</td><td>
<?php print $request->protocol ?>
</td></tr><tr><td>
Path Info:
</td><td>
<?php print $request->pathInfo ?>
</td></tr><tr><td>
Remote Address:
</td><td>
<?php print $request->remoteAddr ?>
</table>

